package ui;

import service.GameEngine;
import service.LeaderboardService;

import javax.swing.SwingUtilities;
import javax.swing.UIManager;

/**
 * Standalone launcher for the AirStriker Java Swing Desktop Application.
 * Entry point: public static void main(String[] args).
 */
public class SwingGameLauncher {
    public static void main(String[] args) {
        System.out.println("====================================================================");
        System.out.println("   LAUNCHING AIRSTRIKER: JAVA SWING DESKTOP ARCADE SHOOTER          ");
        System.out.println("   Module: Problem Solving Using Object Oriented Programming (PSOOP)");
        System.out.println("====================================================================");

        // Ensure Swing UI components are created on the Event Dispatch Thread (EDT)
        SwingUtilities.invokeLater(() -> {
            try {
                // Set system or modern Nimbus look and feel if available
                for (UIManager.LookAndFeelInfo info : UIManager.getInstalledLookAndFeels()) {
                    if ("Nimbus".equalsIgnoreCase(info.getName())) {
                        UIManager.setLookAndFeel(info.getClassName());
                        break;
                    }
                }
            } catch (Exception e) {
                // Fallback to default look and feel gracefully
            }

            final LeaderboardService leaderboardService = new LeaderboardService();
            try {
                leaderboardService.loadLeaderboard();
            } catch (Exception e) {
                System.err.println("[SwingGameLauncher] Note: Default leaderboards initialized.");
            }

            // Asynchronously synchronize live Supabase cloud records at launch
            java.util.concurrent.CompletableFuture.runAsync(() -> {
                try {
                    boolean ok = leaderboardService.syncWithCloud();
                    if (ok) {
                        System.out.println("[SwingGameLauncher] >>> Supabase Cloud Live Leaderboard synchronized successfully.");
                    }
                } catch (Exception ignored) {}
            });

            final GameEngine engine = new GameEngine("StarViper");
            engine.spawnWave(4);

            final GameWindow window = new GameWindow(engine, leaderboardService);
            window.setVisible(true);
            window.getGamePanel().requestFocusInWindow();

            System.out.println("[SwingGameLauncher] >>> Game window displayed at 850x620 resolution (60 FPS).");
        });
    }
}
