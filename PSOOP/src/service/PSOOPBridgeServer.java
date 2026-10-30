package service;

import model.Boss;
import model.Drone;
import model.Enemy;
import model.Fighter;
import model.GameState;
import model.Player;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketTimeoutException;
import java.util.List;

/**
 * AirStriker Inter-Module Bridge Server (PSOOP Java Subsystem)
 * 
 * Provides high-speed TCP loopback IPC on 127.0.0.1:9092 connecting PSOOP with:
 * 1. PL (C++): BulletPoolArray & EnemyLinkedList
 * 2. COA (80386 ALU): Register file (EAX, EDX) & Condition Flags (ZF, SF, CF)
 * 3. CGL (C++ / OpenGL): 60 FPS Render Loop & HUD Telemetry
 */
public class PSOOPBridgeServer {
    public static final int DEFAULT_PORT = 9092;
    private final int port;
    private GameEngine gameEngine;
    private final LeaderboardService leaderboardService;
    private boolean scoreSubmitted;
    private volatile boolean running;

    public PSOOPBridgeServer() {
        this(DEFAULT_PORT);
    }

    public PSOOPBridgeServer(final int port) {
        this.port = port;
        this.gameEngine = new GameEngine("CadetPilot");
        this.leaderboardService = new LeaderboardService();
        this.scoreSubmitted = false;
        this.running = true;
        try {
            this.leaderboardService.loadLeaderboard();
        } catch (Exception ignored) {}
    }

    public void start() {
        System.out.println("====================================================================");
        System.out.println("   AIRSTRIKER: PSOOP BRIDGE SERVER (Java OOP Domain Engine)        ");
        System.out.println("   Listening on localhost:" + port + " for CGL / PL / COA C++ Clients");
        System.out.println("====================================================================");

        try (ServerSocket serverSocket = new ServerSocket(port)) {
            serverSocket.setReuseAddress(true);

            while (running) {
                System.out.println("\n[PSOOP SERVER] Waiting for CGL OpenGL client connection...");
                try {
                    final Socket clientSocket = serverSocket.accept();
                    clientSocket.setTcpNoDelay(true);
                    System.out.println("[PSOOP SERVER] >>> C++ Native Engine Connected: " + clientSocket.getRemoteSocketAddress());
                    handleClient(clientSocket);
                } catch (Exception e) {
                    if (running) {
                        System.err.println("[PSOOP SERVER] Client connection error: " + e.getMessage());
                    }
                }
            }
        } catch (Exception e) {
            System.err.println("[PSOOP SERVER FATAL] Could not listen on port " + port + ": " + e.getMessage());
        }
    }

    private void handleClient(final Socket socket) {
        try (
            BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            PrintWriter out = new PrintWriter(socket.getOutputStream(), true)
        ) {
            String line;
            while (running && (line = in.readLine()) != null) {
                line = line.trim();
                if (line.isEmpty()) continue;

                final String response = processCommand(line);
                out.println(response);

                if ("QUIT".equalsIgnoreCase(line) || "EXIT".equalsIgnoreCase(line)) {
                    break;
                }
            }
        } catch (Exception e) {
            System.out.println("[PSOOP SERVER] Client disconnected: " + e.getMessage());
        } finally {
            try {
                socket.close();
            } catch (Exception ignored) {}
        }
    }

    public String processCommand(final String cmdLine) {
        final String[] tokens = cmdLine.split("\\s+");
        final String action = tokens[0].toUpperCase();

        switch (action) {
            case "PING":
                return "PONG PSOOP_ONLINE_JDK26";

            case "INIT": {
                final String pilot = tokens.length > 1 ? tokens[1] : "CadetPilot";
                this.gameEngine = new GameEngine(pilot);
                this.gameEngine.spawnWave(4);
                return "INIT_OK " + serializeState();
            }

            case "SPAWN_WAVE": {
                int waveNum = 1;
                if (tokens.length > 1) {
                    try { waveNum = Integer.parseInt(tokens[1]); } catch (Exception ignored) {}
                }
                this.gameEngine.spawnWave(3 + waveNum);
                return "SPAWN_OK " + serializeEnemies();
            }

            case "MOVE_PLAYER": {
                double dx = 0.0, dy = 0.0;
                if (tokens.length >= 3) {
                    try {
                        dx = Double.parseDouble(tokens[1]);
                        dy = Double.parseDouble(tokens[2]);
                    } catch (Exception ignored) {}
                }
                gameEngine.movePlayer(dx, dy);
                final Player p = gameEngine.getPlayer();
                return String.format("PLAYER_POS %.2f %.2f", p.getX(), p.getY());
            }

            case "TICK": {
                // Polymorphic enemy kinematic update inside Java OOP hierarchy
                final List<Enemy> enemies = gameEngine.getEnemies();
                for (final Enemy e : enemies) {
                    e.move(); // Drone oscillates, Fighter sweeps, Boss advances
                }
                enemies.removeIf(e -> !e.isActive());

                return "TICK_OK " + serializeState() + " ##ENEMIES## " + serializeEnemies();
            }

            case "ALU_FEEDBACK": {
                // Cross-Module Communication with COA 80386 ALU
                // Format: ALU_FEEDBACK <damage> <scoreBounty> <eax> <edx> <zf> <sf> <cf>
                int damage = 0;
                int bounty = 0;
                int zf = 0;
                int sf = 0;
                if (tokens.length >= 7) {
                    try {
                        damage = Integer.parseInt(tokens[1]);
                        bounty = Integer.parseInt(tokens[2]);
                        zf = Integer.parseInt(tokens[5]);
                        sf = Integer.parseInt(tokens[6]);
                    } catch (Exception ignored) {}
                }

                final Player player = gameEngine.getPlayer();
                if (damage > 0) {
                    player.takeDamage(damage);
                    if (zf == 1 || sf == 1 || player.getHealth() <= 0) {
                        player.setHealth(0);
                        player.setActive(false);
                        if (!scoreSubmitted) {
                            scoreSubmitted = true;
                            System.out.printf("[PSOOP SERVER] >>> Game Over Detected! Auto-submitting score %d for %s (Wave %d) to Supabase Cloud...%n",
                                    player.getScore(), player.getName(), gameEngine.getCurrentWave());
                            leaderboardService.recordScore(player.getName(), player.getScore(), gameEngine.getCurrentWave());
                            try {
                                leaderboardService.saveLeaderboard();
                            } catch (Exception ignored) {}
                        }
                    }
                }

                if (bounty > 0) {
                    player.addScore(bounty);
                }

                return String.format("ALU_SYNC_OK HP=%d SHIELD=%d SCORE=%d OVER=%b",
                        player.getHealth(), player.getShield(), player.getScore(), !player.isAlive());
            }

            case "GET_STATE":
                return serializeState();

            case "GET_ENEMIES":
                return serializeEnemies();

            case "RESET": {
                this.gameEngine = new GameEngine("CadetPilot");
                this.gameEngine.spawnWave(4);
                this.scoreSubmitted = false;
                return "RESET_OK " + serializeState();
            }

            case "QUIT":
            case "EXIT":
                return "BYE";

            default:
                return "ERR UNKNOWN_COMMAND: " + action;
        }
    }

    private String serializeState() {
        final GameState s = gameEngine.getGameState();
        return String.format("STATE px=%.2f py=%.2f hp=%d maxHp=100 sh=%d maxSh=50 sc=%d wv=%d over=%b hud=\"%s\"",
                s.getPlayerX(), s.getPlayerY(), s.getHealth(), s.getShield(), s.getScore(), s.getWave(),
                s.isGameOver(), s.getHudDisplayText());
    }

    private String serializeEnemies() {
        final List<Enemy> enemies = gameEngine.getEnemies();
        final StringBuilder sb = new StringBuilder();
        sb.append("ENEMIES count=").append(enemies.size());

        for (final Enemy e : enemies) {
            sb.append(";").append(e.getId())
              .append(",").append(e.getEnemyType())
              .append(",").append(String.format("%.1f", e.getX()))
              .append(",").append(String.format("%.1f", e.getY()))
              .append(",").append(e.getHealth())
              .append(",").append(e.getMaxHealth())
              .append(",").append(e.getScoreValue())
              .append(",").append(e.getCollisionDamage());
        }
        return sb.toString();
    }

    public void stop() {
        this.running = false;
    }

    public static void main(final String[] args) {
        int port = DEFAULT_PORT;
        if (args.length > 0) {
            try {
                port = Integer.parseInt(args[0]);
            } catch (Exception ignored) {}
        }
        final PSOOPBridgeServer server = new PSOOPBridgeServer(port);
        server.start();
    }
}
