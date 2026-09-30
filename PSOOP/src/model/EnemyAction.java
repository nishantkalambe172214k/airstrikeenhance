package model;

// CO-2: Custom Functional Interface for Lambda Expressions
@FunctionalInterface
public interface EnemyAction {
    void execute(Enemy enemy);
}
