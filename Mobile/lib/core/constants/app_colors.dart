import 'package:flutter/material.dart';

// ============================================================================
// App Colors - Christmas Theme
// ============================================================================

class AppColors {
  // Primary Colors
  static const Color christmasRed = Color(0xFFD32F2F);
  static const Color forestGreen = Color(0xFF2E7D32);
  static const Color gold = Color(0xFFFFC107);
  static const Color snowWhite = Color(0xFFFAFAFA);
  static const Color midnightBlue = Color(0xFF1A237E);

  // LED Control Colors
  static const Color warmWhite = Color(0xFFFFF8DC);
  static const Color coolWhite = Color(0xFFF0F8FF);

  // Status Colors
  static const Color success = Color(0xFF4CAF50);
  static const Color warning = Color(0xFFFF9800);
  static const Color error = Color(0xFFF44336);
  static const Color info = Color(0xFF2196F3);

  // Gradients
  static const LinearGradient christmasGradient = LinearGradient(
    colors: [christmasRed, forestGreen],
    begin: Alignment.topLeft,
    end: Alignment.bottomRight,
  );

  static const LinearGradient goldGradient = LinearGradient(
    colors: [Color(0xFFFFD700), Color(0xFFFFA500)],
    begin: Alignment.topCenter,
    end: Alignment.bottomCenter,
  );

  // Dark Mode
  static const Color darkBackground = Color(0xFF121212);
  static const Color darkSurface = Color(0xFF1E1E1E);
  static const Color darkCard = Color(0xFF2C2C2C);
}
