package model;

// CO-2: Custom Functional Interface with return value for Lambda Expressions
@FunctionalInterface
public interface ScoreCalculator {
    int calculate(int baseScore, int comboStreak);
}
