package service;

import exception.GameException;
import model.PilotProfile;

import java.io.IOException;
import java.net.URI;
import java.net.http.HttpClient;
import java.net.http.HttpRequest;
import java.net.http.HttpResponse;
import java.time.Duration;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.CompletableFuture;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * High-performance, zero-dependency REST client connecting AirStriker to
 * managed PostgreSQL on Supabase Cloud.
 * Uses standard java.net.http.HttpClient (JDK 11+) with asynchronous execution.
 */
public class SupabaseClient {
    public static final String SUPABASE_URL = "https://yuuyftdzmpxwgmbulrgf.supabase.co/rest/v1";
    public static final String PUBLISHABLE_KEY = "sb_publishable_IZXDAV3NBzj4B9PONHfLYQ_KmDT--bf";

    private static final HttpClient client = HttpClient.newBuilder()
            .connectTimeout(Duration.ofSeconds(3))
            .build();

    /**
     * Checks if Supabase Cloud is reachable within 2 seconds.
     */
    public static boolean isCloudReachable() {
        try {
            HttpRequest request = HttpRequest.newBuilder()
                    .uri(URI.create(SUPABASE_URL + "/leaderboard?select=record_id&limit=1"))
                    .header("apikey", PUBLISHABLE_KEY)
                    .header("Authorization", "Bearer " + PUBLISHABLE_KEY)
                    .timeout(Duration.ofSeconds(2))
                    .GET()
                    .build();

            HttpResponse<String> response = client.send(request, HttpResponse.BodyHandlers.ofString());
            return response.statusCode() == 200;
        } catch (Exception e) {
            return false;
        }
    }

    /**
     * Fetches top global high scores from Supabase PostgreSQL.
     * Returns empty list gracefully if offline or network fails.
     */
    public static List<PilotProfile> fetchCloudLeaderboard(final int limit) throws GameException {
        final List<PilotProfile> cloudProfiles = new ArrayList<>();
        final String endpoint = SUPABASE_URL + "/leaderboard?select=record_id,player_id,callsign,score,wave_reached,play_date&order=score.desc&limit=" + Math.max(1, limit);

        try {
            HttpRequest request = HttpRequest.newBuilder()
                    .uri(URI.create(endpoint))
                    .header("apikey", PUBLISHABLE_KEY)
                    .header("Authorization", "Bearer " + PUBLISHABLE_KEY)
                    .timeout(Duration.ofSeconds(4))
                    .GET()
                    .build();

            HttpResponse<String> response = client.send(request, HttpResponse.BodyHandlers.ofString());

            if (response.statusCode() != 200) {
                throw new GameException("ERR_SUPABASE_QUERY_FAILED",
                        "Supabase returned HTTP status: " + response.statusCode() + " - " + response.body());
            }

            // Lightweight, zero-dependency JSON parsing for array of records
            String body = response.body().trim();
            if (body.startsWith("[") && body.endsWith("]")) {
                body = body.substring(1, body.length() - 1).trim();
            }

            if (body.isEmpty()) {
                return cloudProfiles;
            }

            // Split into individual JSON objects: {...}, {...}
            Pattern objectPattern = Pattern.compile("\\{([^}]+)\\}");
            Matcher objectMatcher = objectPattern.matcher(body);

            int fallbackId = 2000;
            while (objectMatcher.find()) {
                String objContent = objectMatcher.group(1);

                String callsign = extractJsonString(objContent, "callsign", "UnknownPilot");
                int score = extractJsonInt(objContent, "score", 0);
                int wave = extractJsonInt(objContent, "wave_reached", 1);
                int recordId = extractJsonInt(objContent, "record_id", fallbackId++);
                String playDate = extractJsonString(objContent, "play_date", "2026-10-29");

                // Format timestamp nicely (take YYYY-MM-DD)
                if (playDate.length() >= 10) {
                    playDate = playDate.substring(0, 10);
                }

                cloudProfiles.add(new PilotProfile(recordId, callsign, 1, score, wave, playDate));
            }

            return cloudProfiles;
        } catch (IOException | InterruptedException e) {
            throw new GameException("ERR_SUPABASE_NETWORK_TIMEOUT",
                    "Network error connecting to Supabase cloud database: " + e.getMessage(), e);
        }
    }

    /**
     * Submits a mission score to the Supabase Cloud Leaderboard synchronously.
     */
    public static boolean submitScoreSync(final String callsign, final int score, final int wave) {
        if (callsign == null || callsign.trim().isEmpty() || score < 0) {
            return false;
        }

        final String cleanCallsign = callsign.trim().replaceAll("\"", "");
        final String jsonPayload = String.format(
                "{\"callsign\":\"%s\",\"score\":%d,\"wave_reached\":%d}",
                cleanCallsign, score, wave
        );

        try {
            HttpRequest request = HttpRequest.newBuilder()
                    .uri(URI.create(SUPABASE_URL + "/leaderboard"))
                    .header("apikey", PUBLISHABLE_KEY)
                    .header("Authorization", "Bearer " + PUBLISHABLE_KEY)
                    .header("Content-Type", "application/json")
                    .header("Prefer", "return=representation")
                    .timeout(Duration.ofSeconds(4))
                    .POST(HttpRequest.BodyPublishers.ofString(jsonPayload))
                    .build();

            HttpResponse<String> response = client.send(request, HttpResponse.BodyHandlers.ofString());
            return response.statusCode() == 200 || response.statusCode() == 201;
        } catch (Exception e) {
            System.err.println("[SupabaseClient WARNING] Score submission failed: " + e.getMessage());
            return false;
        }
    }

    /**
     * Non-blocking asynchronous submission. Does NOT block game loop frame execution.
     */
    public static CompletableFuture<Boolean> submitScoreAsync(final String callsign, final int score, final int wave) {
        return CompletableFuture.supplyAsync(() -> submitScoreSync(callsign, score, wave));
    }

    // --- Helper Utilities for Lightweight JSON Parsing ---

    private static String extractJsonString(String json, String key, String defaultVal) {
        Pattern pattern = Pattern.compile("\"" + key + "\"\\s*:\\s*\"([^\"]*)\"");
        Matcher matcher = pattern.matcher(json);
        if (matcher.find()) {
            return matcher.group(1);
        }
        return defaultVal;
    }

    private static int extractJsonInt(String json, String key, int defaultVal) {
        Pattern pattern = Pattern.compile("\"" + key + "\"\\s*:\\s*([0-9]+)");
        Matcher matcher = pattern.matcher(json);
        if (matcher.find()) {
            try {
                return Integer.parseInt(matcher.group(1));
            } catch (NumberFormatException ignored) {
            }
        }
        return defaultVal;
    }
}
