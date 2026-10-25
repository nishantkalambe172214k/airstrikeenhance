package model;

// ============================================================================
// CO-2: Interface Contract Definition
// CO-3: Combat System - Movement Contract for Dynamic Polymorphism
// Defines the core movement behavior contract required for all active combatants
// (Player, Enemy, Drone, Fighter, Boss) in the 2D combat arena.
// ============================================================================
public interface Movable {
    void move();
}
