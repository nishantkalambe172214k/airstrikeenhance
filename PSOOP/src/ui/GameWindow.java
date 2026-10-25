package ui;

import service.GameEngine;
import service.LeaderboardService;

import javax.swing.JFrame;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.KeyStroke;
import java.awt.BorderLayout;
import java.awt.event.KeyEvent;

/**
 * Main application window (JFrame) for AirStriker PSOOP Java Subsystem.
 * Demonstrates:
 * 1. Java Swing: JFrame, JMenuBar, JMenu, JMenuItem, KeyStroke accelerators, JOptionPane.
 * 2. Modular Architecture: ui package encapsulating Swing views and communicating with service.
 */
public class GameWindow extends JFrame {
    private final GameEngine engine;
    private final LeaderboardService leaderboardService;
    private final GamePanel gamePanel;

    public GameWindow(final GameEngine engine, final LeaderboardService leaderboardService) {
        super("AirStriker — 2D Space Shooter (PSOOP Java Subsystem)");
        this.engine = engine;
        this.leaderboardService = leaderboardService;
        this.gamePanel = new GamePanel(engine, leaderboardService);

        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setResizable(false);
        setLayout(new BorderLayout());

        // Setup Swing Menu Bar
        setJMenuBar(createMenuBar());

        // Embed Game Canvas
        add(gamePanel, BorderLayout.CENTER);

        pack();
        setLocationRelativeTo(null);
    }

    private JMenuBar createMenuBar() {
        JMenuBar menuBar = new JMenuBar();

        // 1. Mission Menu
        JMenu missionMenu = new JMenu("Mission");
        missionMenu.setMnemonic(KeyEvent.VK_M);

        JMenuItem newGameItem = new JMenuItem("New Pilot Mission");
        newGameItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_N, KeyEvent.CTRL_DOWN_MASK));
        newGameItem.addActionListener(e -> promptNewPilotMission());
        missionMenu.add(newGameItem);

        JMenuItem pauseItem = new JMenuItem("Pause / Resume");
        pauseItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_P, 0));
        pauseItem.addActionListener(e -> gamePanel.togglePause());
        missionMenu.add(pauseItem);

        JMenuItem restartItem = new JMenuItem("Restart Mission");
        restartItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_R, 0));
        restartItem.addActionListener(e -> gamePanel.restartGame());
        missionMenu.add(restartItem);

        missionMenu.addSeparator();

        JMenuItem exitItem = new JMenuItem("Exit AirStriker");
        exitItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_Q, KeyEvent.CTRL_DOWN_MASK));
        exitItem.addActionListener(e -> {
            int confirm = JOptionPane.showConfirmDialog(this,
                    "Are you sure you want to abort the current mission and exit?",
                    "Confirm Exit",
                    JOptionPane.YES_NO_OPTION,
                    JOptionPane.QUESTION_MESSAGE);
            if (confirm == JOptionPane.YES_OPTION) {
                System.exit(0);
            }
        });
        missionMenu.add(exitItem);

        menuBar.add(missionMenu);

        // 2. Telemetry & High Scores Menu
        JMenu statsMenu = new JMenu("Telemetry");
        statsMenu.setMnemonic(KeyEvent.VK_T);

        JMenuItem leaderboardItem = new JMenuItem("Pilot Leaderboard (Hall of Fame)");
        leaderboardItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_H, 0));
        leaderboardItem.addActionListener(e -> showLeaderboard());
        statsMenu.add(leaderboardItem);

        menuBar.add(statsMenu);

        // 3. Help & Academic Outlining Menu
        JMenu helpMenu = new JMenu("Help");
        helpMenu.setMnemonic(KeyEvent.VK_H);

        JMenuItem controlsItem = new JMenuItem("Flight Controls (F1)");
        controlsItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_F1, 0));
        controlsItem.addActionListener(e -> showFlightControls());
        helpMenu.add(controlsItem);

        JMenuItem syllabusItem = new JMenuItem("Academic Syllabus Mapping");
        syllabusItem.addActionListener(e -> showSyllabusMapping());
        helpMenu.add(syllabusItem);

        menuBar.add(helpMenu);

        return menuBar;
    }

    private void promptNewPilotMission() {
        String callsign = JOptionPane.showInputDialog(this,
                "Enter your Pilot Callsign for this combat mission:",
                "AirStriker Pilot Registry",
                JOptionPane.QUESTION_MESSAGE);

        if (callsign != null && !callsign.trim().isEmpty()) {
            engine.getPlayer().setName(callsign.trim());
            gamePanel.restartGame();
        }
    }

    private void showLeaderboard() {
        LeaderboardDialog dialog = new LeaderboardDialog(this, leaderboardService);
        dialog.setVisible(true);
    }

    private void showFlightControls() {
        String message = """
                =============================================
                     AIRSTRIKER 2D DESKTOP FLIGHT CONTROLS
                =============================================
                
                  [W] or [UP]       : Move Ship Upward
                  [S] or [DOWN]     : Move Ship Downward
                  [A] or [LEFT]     : Move Ship Left (Retreat)
                  [D] or [RIGHT]    : Move Ship Right (Advance)
                  [SPACEBAR]        : Fire Laser Cannons
                
                  [P]               : Pause / Resume Mission
                  [R]               : Quick Mission Restart
                  [H]               : Open Pilot Hall of Fame
                  [F1]              : Show This Controls Guide
                
                =============================================
                  WEAPON HEAT SYSTEM:
                  Continuous firing will trigger a WeaponOverheatException
                  and temporary lockout until heat dissipates!
                =============================================
                """;
        JOptionPane.showMessageDialog(this, message, "Flight Controls", JOptionPane.INFORMATION_MESSAGE);
    }

    private void showSyllabusMapping() {
        String message = """
                AirStriker: Problem Solving Using OOP (PSOOP)
                Integrated Academic Syllabus Implementations:
                
                1. Exception Handling:
                   - Custom Checked Exceptions: GameException, WeaponOverheatException,
                     ShieldDepletedException, GameOverException.
                   - Custom Unchecked Exception: InvalidEntityStateException.
                   - try, catch, finally, throw, throws across domain logic.
                
                2. Java Swing Desktop GUI:
                   - JFrame (GameWindow), JPanel (GamePanel), JDialog (LeaderboardDialog).
                   - Custom 2D double-buffered paintComponent() with 60 FPS Timer.
                   - KeyListener with multi-key diagonal tracking.
                
                3. Packages & Modular Architecture:
                   - model, service, exception, ui, test.
                
                4. Java Collections Framework:
                   - List, ArrayList, Map, HashMap, Set, HashSet, Queue, Deque,
                     Iterator, Collections.sort(), Comparators.
                """;
        JOptionPane.showMessageDialog(this, message, "Academic Syllabus Mapping", JOptionPane.INFORMATION_MESSAGE);
    }

    public GamePanel getGamePanel() {
        return gamePanel;
    }
}
