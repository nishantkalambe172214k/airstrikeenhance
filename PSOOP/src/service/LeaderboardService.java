package service;

import exception.GameException;
import model.PilotProfile;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * Service managing pilot leaderboards and persistence.
 * Demonstrates:
 * 1. Java Collections Framework: List, Map, HashMap, Collections.sort, custom Comparators.
 * 2. Exception Handling: Checked GameException, try-catch-finally, try-with-resources, recovery.
 * 3. Modular Package Architecture: service interacts with model, exception.
 */
public class LeaderboardService {
    private final File leaderboardFile;
    private final List<PilotProfile> profiles;
    private final Map<String, Integer> highScoresByCallsign;
    private final Map<Integer, PilotProfile> profilesById;

    public LeaderboardService() {
        this(new File("data/leaderboard.csv"));
    }

    public LeaderboardService(final File leaderboardFile) {
        this.leaderboardFile = leaderboardFile;
        this.profiles = new ArrayList<>();
        this.highScoresByCallsign = new HashMap<>();
        this.profilesById = new HashMap<>();
    }

    /**
     * Loads pilot records from CSV using try-with-resources and exception handling.
     * Throws checked GameException if critical format failure occurs.
     */
    public void loadLeaderboard() throws GameException {
        if (!leaderboardFile.exists()) {
            // Graceful initialization if file does not exist yet
            initDefaultProfiles();
            return;
        }

        profiles.clear();
        highScoresByCallsign.clear();
        profilesById.clear();

        BufferedReader reader = null;
        try {
            reader = new BufferedReader(new FileReader(leaderboardFile));
            String header = reader.readLine(); // Skip header
            String line;
            int lineNumber = 1;

            while ((line = reader.readLine()) != null) {
                lineNumber++;
                line = line.trim();
                if (line.isEmpty()) continue;

                // Expected CSV: Rank,PlayerID,Callsign,Score,WaveReached,Date
                String[] tokens = line.split(",");
                if (tokens.length < 5) {
                    throw new GameException("ERR_CORRUPT_LEADERBOARD",
                            String.format("Corrupt CSV row at line %d: %s", lineNumber, line));
                }

                try {
                    int playerId = Integer.parseInt(tokens[1].trim());
                    String callsign = tokens[2].trim();
                    int score = Integer.parseInt(tokens[3].trim());
                    int waveReached = Integer.parseInt(tokens[4].trim());
                    String date = (tokens.length > 5) ? tokens[5].trim() : "2026-09-01";

                    PilotProfile profile = new PilotProfile(playerId, callsign, 1, score, waveReached, date);
                    profiles.add(profile);
                    profilesById.put(playerId, profile);
                    highScoresByCallsign.put(callsign.toUpperCase(), score);
                } catch (NumberFormatException nfe) {
                    System.err.printf("[LeaderboardService WARNING] Skipping unparseable row at line %d: %s%n",
                            lineNumber, nfe.getMessage());
                }
            }
        } catch (IOException ioe) {
            throw new GameException("ERR_IO_FAILURE", "Failed to read leaderboard file: " + leaderboardFile.getPath(), ioe);
        } finally {
            // Explicit finally block demonstrating resource cleanup
            if (reader != null) {
                try {
                    reader.close();
                } catch (IOException e) {
                    System.err.println("[LeaderboardService] Error closing reader: " + e.getMessage());
                }
            }
        }

        // Sort loaded profiles in descending order using Collections.sort
        Collections.sort(profiles);
    }

    /**
     * Saves current leaderboards to CSV using try-with-resources.
     */
    public void saveLeaderboard() throws GameException {
        if (leaderboardFile.getParentFile() != null && !leaderboardFile.getParentFile().exists()) {
            leaderboardFile.getParentFile().mkdirs();
        }

        // Use try-with-resources for auto-closing PrintWriter
        try (PrintWriter writer = new PrintWriter(new FileWriter(leaderboardFile))) {
            writer.println("Rank,PlayerID,Callsign,Score,WaveReached,Date");
            int rank = 1;
            for (PilotProfile p : profiles) {
                writer.printf("%d,%d,%s,%d,%d,%s%n",
                        rank++, p.getPlayerId(), p.getCallsign(), p.getHighScore(), p.getHighestWave(), p.getCreatedAt());
            }
        } catch (IOException ioe) {
            throw new GameException("ERR_SAVE_FAILED", "Unable to persist leaderboard to " + leaderboardFile.getPath(), ioe);
        }
    }

    /**
     * Synchronizes local leaderboard with Supabase Cloud PostgreSQL.
     * Fetches top global high scores and merges them into memory and local CSV.
     * @return true if cloud sync succeeded, false if offline/fallback.
     */
    public boolean syncWithCloud() {
        try {
            List<PilotProfile> cloudProfiles = SupabaseClient.fetchCloudLeaderboard(25);
            if (cloudProfiles == null || cloudProfiles.isEmpty()) {
                return false;
            }

            for (PilotProfile cloudP : cloudProfiles) {
                final String key = cloudP.getCallsign().toUpperCase();
                Integer localScore = highScoresByCallsign.get(key);

                if (localScore == null) {
                    profiles.add(cloudP);
                    profilesById.put(cloudP.getPlayerId(), cloudP);
                    highScoresByCallsign.put(key, cloudP.getHighScore());
                } else if (cloudP.getHighScore() > localScore) {
                    highScoresByCallsign.put(key, cloudP.getHighScore());
                    for (PilotProfile localP : profiles) {
                        if (localP.getCallsign().equalsIgnoreCase(cloudP.getCallsign())) {
                            localP.updateHighScore(cloudP.getHighScore());
                            localP.setHighestWave(cloudP.getHighestWave());
                            break;
                        }
                    }
                }
            }

            Collections.sort(profiles);
            try {
                saveLeaderboard();
            } catch (GameException ignored) {
            }
            return true;
        } catch (Exception e) {
            System.err.println("[LeaderboardService WARNING] Cloud sync failed (offline mode): " + e.getMessage());
            return false;
        }
    }

    /**
     * Records or updates a pilot's mission result.
     * Uses Collections: Map lookup and List update, then re-sorts.
     * Asynchronously dispatches the score to Supabase PostgreSQL cloud.
     */
    public boolean recordScore(final String callsign, final int score, final int wave) {
        if (callsign == null || callsign.trim().isEmpty() || score < 0) {
            return false;
        }

        // Asynchronously dispatch score to Supabase PostgreSQL cloud in background
        SupabaseClient.submitScoreAsync(callsign, score, wave);

        final String key = callsign.trim().toUpperCase();
        Integer existingScore = highScoresByCallsign.get(key);

        if (existingScore != null) {
            if (score > existingScore) {
                highScoresByCallsign.put(key, score);
                for (PilotProfile p : profiles) {
                    if (p.getCallsign().equalsIgnoreCase(callsign)) {
                        p.updateHighScore(score);
                        p.setHighestWave(wave);
                        p.incrementGamesPlayed();
                        break;
                    }
                }
                Collections.sort(profiles);
                return true;
            }
            return false;
        } else {
            // New pilot
            int newId = 1000 + profiles.size() + 1;
            PilotProfile newProfile = new PilotProfile(newId, callsign.trim(), 1, score, wave, "2026-10-25");
            profiles.add(newProfile);
            profilesById.put(newId, newProfile);
            highScoresByCallsign.put(key, score);
            Collections.sort(profiles);
            return true;
        }
    }

    public List<PilotProfile> getRankedProfiles() {
        return Collections.unmodifiableList(profiles);
    }

    public List<PilotProfile> getProfilesSortedByCallsign() {
        List<PilotProfile> copy = new ArrayList<>(profiles);
        copy.sort(Comparator.comparing(PilotProfile::getCallsign, String.CASE_INSENSITIVE_ORDER));
        return copy;
    }

    public Map<String, Integer> getHighScoresByCallsign() {
        return Collections.unmodifiableMap(highScoresByCallsign);
    }

    public PilotProfile getProfileById(final int id) {
        return profilesById.get(id);
    }

    public int getTopScore() {
        if (profiles.isEmpty()) return 0;
        return profiles.get(0).getHighScore();
    }

    private void initDefaultProfiles() {
        profiles.clear();
        profiles.add(new PilotProfile(1003, "SkyCommander", 22, 6400, 6, "2026-09-03"));
        profiles.add(new PilotProfile(1001, "StarViper", 14, 3500, 4, "2026-09-01"));
        profiles.add(new PilotProfile(1002, "AcesHigh", 9, 2800, 3, "2026-09-02"));
        for (PilotProfile p : profiles) {
            profilesById.put(p.getPlayerId(), p);
            highScoresByCallsign.put(p.getCallsign().toUpperCase(), p.getHighScore());
        }
        Collections.sort(profiles);
    }
}
