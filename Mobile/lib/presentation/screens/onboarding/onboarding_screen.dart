import 'package:flutter/material.dart';
import 'package:shared_preferences/shared_preferences.dart';
import '../../../core/constants/app_colors.dart';
import '../../../l10n/app_localizations.dart';
import '../connection_mode/connection_mode_screen.dart';

// ============================================================================
// Onboarding Screen - 3 Slides Introduction
// ============================================================================

class OnboardingScreen extends StatefulWidget {
  const OnboardingScreen({super.key});

  @override
  State<OnboardingScreen> createState() => _OnboardingScreenState();
}

class _OnboardingScreenState extends State<OnboardingScreen> {
  final PageController _pageController = PageController();
  int _currentPage = 0;
  bool _dontShowAgain = false;

  Future<void> _completeOnboarding() async {
    if (_dontShowAgain) {
      final prefs = await SharedPreferences.getInstance();
      await prefs.setBool('skip_onboarding', true);
    }

    if (mounted) {
      Navigator.of(context).pushReplacement(
        MaterialPageRoute(builder: (_) => const ConnectionModeScreen()),
      );
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: Stack(
        children: [
          // Pages
          PageView(
            controller: _pageController,
            onPageChanged: (index) {
              setState(() {
                _currentPage = index;
              });
            },
            children: [
              _OnboardingPage(
                icon: Icons.park,
                title:
                    AppLocalizations.of(context)?.onboardingWelcomeTitle ??
                    'Welcome to PixelTree! 🎄',
                description:
                    AppLocalizations.of(context)?.onboardingWelcomeBody ??
                    'Transform your Christmas with smart ARGB lights',
                gradient: AppColors.christmasGradient,
              ),
              _OnboardingPage(
                icon: Icons.wifi,
                title:
                    AppLocalizations.of(context)?.onboardingSetupTitle ??
                    'Simple WiFi Setup',
                description:
                    AppLocalizations.of(context)?.onboardingSetupBody ??
                    'Connect your lights to WiFi in just a few taps',
                gradient: const LinearGradient(
                  colors: [AppColors.info, AppColors.forestGreen],
                  begin: Alignment.topLeft,
                  end: Alignment.bottomRight,
                ),
              ),
              _OnboardingPage(
                icon: Icons.color_lens,
                title:
                    AppLocalizations.of(context)?.onboardingControlTitle ??
                    'Endless Possibilities',
                description:
                    AppLocalizations.of(context)?.onboardingControlBody ??
                    'Choose colors, effects, and create magic',
                gradient: AppColors.goldGradient,
              ),
            ],
          ),

          // Bottom controls
          Positioned(
            bottom: 50,
            left: 0,
            right: 0,
            child: Column(
              children: [
                // "Don't show again" checkbox - only on last page, above indicators
                if (_currentPage == 2) ...[
                  Padding(
                    padding: const EdgeInsets.symmetric(horizontal: 32),
                    child: GestureDetector(
                      onTap: () =>
                          setState(() => _dontShowAgain = !_dontShowAgain),
                      child: Row(
                        mainAxisAlignment: MainAxisAlignment.center,
                        children: [
                          Container(
                            width: 22,
                            height: 22,
                            decoration: BoxDecoration(
                              color: _dontShowAgain
                                  ? Colors.white
                                  : Colors.white.withValues(alpha: 0.2),
                              borderRadius: BorderRadius.circular(6),
                              border: Border.all(color: Colors.white, width: 2),
                            ),
                            child: _dontShowAgain
                                ? const Icon(
                                    Icons.check,
                                    size: 16,
                                    color: AppColors.forestGreen,
                                  )
                                : null,
                          ),
                          const SizedBox(width: 10),
                          Text(
                            AppLocalizations.of(
                                  context,
                                )?.onboardingDontShowAgain ??
                                "Don't show this again",
                            style: TextStyle(
                              color: Colors.white.withValues(alpha: 0.9),
                              fontSize: 14,
                            ),
                          ),
                        ],
                      ),
                    ),
                  ),
                  const SizedBox(height: 24),
                ],

                // Page indicators
                Row(
                  mainAxisAlignment: MainAxisAlignment.center,
                  children: List.generate(
                    3,
                    (index) => AnimatedContainer(
                      duration: const Duration(milliseconds: 300),
                      margin: const EdgeInsets.symmetric(horizontal: 4),
                      width: _currentPage == index ? 32 : 8,
                      height: 8,
                      decoration: BoxDecoration(
                        color: _currentPage == index
                            ? Colors.white
                            : Colors.white.withValues(alpha: 0.5),
                        borderRadius: BorderRadius.circular(4),
                      ),
                    ),
                  ),
                ),

                const SizedBox(height: 32),

                // Button
                Padding(
                  padding: const EdgeInsets.symmetric(horizontal: 32),
                  child: ElevatedButton(
                    onPressed: () {
                      if (_currentPage < 2) {
                        _pageController.nextPage(
                          duration: const Duration(milliseconds: 300),
                          curve: Curves.easeInOut,
                        );
                      } else {
                        _completeOnboarding();
                      }
                    },
                    style: ElevatedButton.styleFrom(
                      backgroundColor: Colors.white,
                      foregroundColor: AppColors.forestGreen,
                      minimumSize: const Size(double.infinity, 56),
                    ),
                    child: Text(
                      _currentPage < 2
                          ? (AppLocalizations.of(context)?.buttonContinue ??
                                'Continue')
                          : (AppLocalizations.of(
                                  context,
                                )?.onboardingGetStarted ??
                                "Let's Get Started!"),
                    ),
                  ),
                ),
              ],
            ),
          ),
        ],
      ),
    );
  }

  @override
  void dispose() {
    _pageController.dispose();
    super.dispose();
  }
}

// ============================================================================
// Onboarding Page Widget
// ============================================================================

class _OnboardingPage extends StatelessWidget {
  final IconData icon;
  final String title;
  final String description;
  final Gradient gradient;

  const _OnboardingPage({
    required this.icon,
    required this.title,
    required this.description,
    required this.gradient,
  });

  @override
  Widget build(BuildContext context) {
    return Container(
      decoration: BoxDecoration(gradient: gradient),
      child: SafeArea(
        child: Padding(
          padding: const EdgeInsets.all(32.0),
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              // Icon
              Icon(icon, size: 120, color: Colors.white),

              const SizedBox(height: 48),

              // Title
              Text(
                title,
                style: Theme.of(context).textTheme.displayMedium?.copyWith(
                  color: Colors.white,
                  fontWeight: FontWeight.bold,
                ),
                textAlign: TextAlign.center,
              ),

              const SizedBox(height: 24),

              // Description
              Text(
                description,
                style: Theme.of(context).textTheme.bodyLarge?.copyWith(
                  color: Colors.white.withValues(alpha: 0.9),
                  height: 1.5,
                ),
                textAlign: TextAlign.center,
              ),
            ],
          ),
        ),
      ),
    );
  }
}
