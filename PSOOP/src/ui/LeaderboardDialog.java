package ui;

import exception.GameException;
import model.PilotProfile;
import service.LeaderboardService;

import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.SwingConstants;
import javax.swing.table.DefaultTableCellRenderer;
import javax.swing.table.DefaultTableModel;
import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.Font;
import java.util.List;

/**
 * Swing modal dialog displaying top pilot rankings.
 * Demonstrates:
 * 1. Java Swing: JDialog, JTable, DefaultTableModel, custom colors and borders.
 * 2. Java Collections: Iterates over List<PilotProfile> sorted with Collections.sort().
 * 3. Exception Handling: Catches GameException during leaderboard reloading.
 */
public class LeaderboardDialog extends JDialog {
    private final LeaderboardService leaderboardService;
    private final DefaultTableModel tableModel;
    private final JTable table;

    public LeaderboardDialog(final JFrame parent, final LeaderboardService service) {
        super(parent, "AirStriker — Pilot Hall of Fame & Leaderboard", true);
        this.leaderboardService = service;

        setLayout(new BorderLayout(10, 10));
        setSize(560, 400);
        setLocationRelativeTo(parent);
        getContentPane().setBackground(new Color(15, 23, 42));

        // Header Container Panel
        JPanel northPanel = new JPanel(new BorderLayout(5, 5));
        northPanel.setBackground(new Color(15, 23, 42));

        // Header Title (JLabel)
        JLabel titleLabel = new JLabel("PILOT TOP SCORES & MISSION RANKINGS", SwingConstants.CENTER);
        titleLabel.setFont(new Font("Monospaced", Font.BOLD, 18));
        titleLabel.setForeground(new Color(250, 204, 21));
        titleLabel.setBorder(BorderFactory.createEmptyBorder(12, 10, 4, 10));
        northPanel.add(titleLabel, BorderLayout.NORTH);

        // Java Swing: Search Bar with JLabel, JTextField, JButton & ActionListener
        JPanel searchPanel = new JPanel(new FlowLayout(FlowLayout.CENTER, 8, 4));
        searchPanel.setBackground(new Color(15, 23, 42));

        JLabel searchLabel = new JLabel("Search Callsign:");
        searchLabel.setFont(new Font("SansSerif", Font.BOLD, 12));
        searchLabel.setForeground(new Color(148, 163, 184));
        searchPanel.add(searchLabel);

        javax.swing.JTextField searchField = new javax.swing.JTextField(12);
        searchField.setFont(new Font("Monospaced", Font.PLAIN, 12));
        searchField.setToolTipText("Enter pilot callsign to search");
        searchPanel.add(searchField);

        JButton searchButton = new JButton("Filter");
        searchButton.setFont(new Font("SansSerif", Font.BOLD, 12));
        searchButton.addActionListener(e -> filterData(searchField.getText()));
        searchPanel.add(searchButton);

        JButton resetButton = new JButton("Reset");
        resetButton.setFont(new Font("SansSerif", Font.BOLD, 12));
        resetButton.addActionListener(e -> {
            searchField.setText("");
            reloadData();
        });
        searchPanel.add(resetButton);

        northPanel.add(searchPanel, BorderLayout.SOUTH);
        add(northPanel, BorderLayout.NORTH);

        // Table Model & JTable
        String[] columns = { "Rank", "Callsign", "High Score", "Max Wave", "Missions", "Enlistment" };
        this.tableModel = new DefaultTableModel(columns, 0) {
            @Override
            public boolean isCellEditable(int row, int column) {
                return false;
            }
        };

        this.table = new JTable(tableModel);
        this.table.setFont(new Font("Monospaced", Font.PLAIN, 13));
        this.table.setRowHeight(26);
        this.table.setBackground(new Color(30, 41, 59));
        this.table.setForeground(new Color(241, 245, 249));
        this.table.setGridColor(new Color(51, 65, 85));
        this.table.getTableHeader().setBackground(new Color(15, 23, 42));
        this.table.getTableHeader().setForeground(new Color(56, 189, 248));
        this.table.getTableHeader().setFont(new Font("Monospaced", Font.BOLD, 13));

        // Center cell renderer
        DefaultTableCellRenderer centerRenderer = new DefaultTableCellRenderer();
        centerRenderer.setHorizontalAlignment(SwingConstants.CENTER);
        for (int i = 0; i < table.getColumnCount(); i++) {
            table.getColumnModel().getColumn(i).setCellRenderer(centerRenderer);
        }

        JScrollPane scrollPane = new JScrollPane(table);
        scrollPane.getViewport().setBackground(new Color(30, 41, 59));
        scrollPane.setBorder(BorderFactory.createLineBorder(new Color(56, 189, 248, 80)));
        add(scrollPane, BorderLayout.CENTER);

        // Bottom Container Panel
        JPanel southPanel = new JPanel(new BorderLayout());
        southPanel.setBackground(new Color(15, 23, 42));

        // Live Cloud Connection Status Indicator
        JLabel statusLabel = new JLabel("  ● Live Cloud: Syncing...", SwingConstants.LEFT);
        statusLabel.setFont(new Font("Monospaced", Font.BOLD, 12));
        statusLabel.setForeground(new Color(250, 204, 21)); // Gold/Yellow
        southPanel.add(statusLabel, BorderLayout.WEST);

        // Button Panel
        JPanel buttonPanel = new JPanel(new FlowLayout(FlowLayout.RIGHT, 12, 10));
        buttonPanel.setBackground(new Color(15, 23, 42));

        JButton cloudSyncButton = new JButton("Sync Cloud (Supabase)");
        cloudSyncButton.setFont(new Font("SansSerif", Font.BOLD, 12));
        cloudSyncButton.setBackground(new Color(16, 185, 129));
        cloudSyncButton.setForeground(new Color(16, 185, 129));
        cloudSyncButton.setToolTipText("Synchronize scores with PostgreSQL on Supabase Cloud");
        cloudSyncButton.addActionListener(e -> triggerAsyncCloudSync(cloudSyncButton, statusLabel, true));
        buttonPanel.add(cloudSyncButton);

        JButton refreshButton = new JButton("Refresh");
        refreshButton.setFont(new Font("SansSerif", Font.BOLD, 12));
        refreshButton.setBackground(new Color(30, 41, 59));
        refreshButton.setForeground(new Color(56, 189, 248));
        refreshButton.addActionListener(e -> reloadData());
        buttonPanel.add(refreshButton);

        JButton closeButton = new JButton("Close");
        closeButton.setFont(new Font("SansSerif", Font.BOLD, 12));
        closeButton.setBackground(new Color(56, 189, 248));
        closeButton.setForeground(Color.BLACK);
        closeButton.addActionListener(e -> dispose());
        buttonPanel.add(closeButton);

        southPanel.add(buttonPanel, BorderLayout.EAST);
        add(southPanel, BorderLayout.SOUTH);

        reloadData();

        // Automatic background live sync upon opening leaderboard
        triggerAsyncCloudSync(cloudSyncButton, statusLabel, false);
    }

    private void triggerAsyncCloudSync(final JButton syncButton, final JLabel statusLabel, final boolean showPopup) {
        syncButton.setEnabled(false);
        syncButton.setText("Syncing Cloud...");
        statusLabel.setText("  ● Live Cloud: Syncing...");
        statusLabel.setForeground(new Color(250, 204, 21));

        java.util.concurrent.CompletableFuture.supplyAsync(() -> leaderboardService.syncWithCloud())
                .thenAccept(ok -> {
                    javax.swing.SwingUtilities.invokeLater(() -> {
                        syncButton.setEnabled(true);
                        syncButton.setText("Sync Cloud (Supabase)");
                        if (ok) {
                            statusLabel.setText("  ● Live Cloud: Connected");
                            statusLabel.setForeground(new Color(16, 185, 129));
                            reloadData();
                            if (showPopup) {
                                javax.swing.JOptionPane.showMessageDialog(this,
                                        "Successfully synchronized with Supabase PostgreSQL Cloud!\nLive records updated in Hall of Fame.",
                                        "Cloud Sync Successful", javax.swing.JOptionPane.INFORMATION_MESSAGE);
                            }
                        } else {
                            statusLabel.setText("  ● Live Cloud: Offline Mode");
                            statusLabel.setForeground(new Color(239, 68, 68));
                            if (showPopup) {
                                javax.swing.JOptionPane.showMessageDialog(this,
                                        "Cloud server unreachable. Operating in local offline cache mode.",
                                        "Offline Mode", javax.swing.JOptionPane.WARNING_MESSAGE);
                            }
                        }
                    });
                });
    }

    public void reloadData() {
        tableModel.setRowCount(0);
        try {
            // Exception Handling: Checked GameException
            leaderboardService.loadLeaderboard();
        } catch (GameException ge) {
            System.err.println("[LeaderboardDialog] Warning loading leaderboard: " + ge.getMessage());
        }

        // Java Collections: Iterating through List<PilotProfile>
        List<PilotProfile> profiles = leaderboardService.getRankedProfiles();
        int rank = 1;
        for (PilotProfile p : profiles) {
            tableModel.addRow(new Object[] {
                    "#" + rank++,
                    p.getCallsign(),
                    p.getHighScore(),
                    p.getHighestWave(),
                    p.getGamesPlayed(),
                    p.getCreatedAt()
            });
        }
    }

    /**
     * Filters JTable records matching search query.
     * Demonstrates Java Collections search iteration over List<PilotProfile>.
     */
    public void filterData(final String query) {
        if (query == null || query.trim().isEmpty()) {
            reloadData();
            return;
        }
        tableModel.setRowCount(0);
        String cleanQuery = query.trim().toLowerCase();
        List<PilotProfile> profiles = leaderboardService.getRankedProfiles();
        int rank = 1;
        for (PilotProfile p : profiles) {
            if (p.getCallsign().toLowerCase().contains(cleanQuery)) {
                tableModel.addRow(new Object[] {
                        "#" + rank,
                        p.getCallsign(),
                        p.getHighScore(),
                        p.getHighestWave(),
                        p.getGamesPlayed(),
                        p.getCreatedAt()
                });
            }
            rank++;
        }
    }
}
