// Effect and Effect Parameter models for PixelTree

/// Effect category enumeration
enum EffectCategory {
  static_(1, 'Statyczne', 'Static'),
  wave(2, 'Fale', 'Wave'),
  chase(3, 'Chase', 'Chase'),
  twinkle(4, 'Twinkle', 'Twinkle'),
  fire(5, 'Ogień', 'Fire'),
  christmas(6, 'Świąteczne', 'Christmas'),
  special(7, 'Specjalne', 'Special'),
  breathing(8, 'Breathing', 'Breathing'),
  alarm(9, 'Alarm', 'Alarm');

  final int id;
  final String nameLocal;
  final String name;

  const EffectCategory(this.id, this.nameLocal, this.name);
}

/// Parameter type enumeration
enum ParameterType {
  uint8,
  bool_,
  color,
  colorArray,
  palette,
  direction,
  enumType,
}

/// Effect parameter definition
class EffectParameter {
  final String id;
  final String name;
  final String nameLocal;
  final ParameterType type;
  final dynamic defaultValue;
  final int? min;
  final int? max;
  final List<String>? enumValues;
  final bool useIncrement; // Use +/- buttons instead of slider
  final String? dependsOn; // Hide this param unless dependsOn param is true

  const EffectParameter({
    required this.id,
    required this.name,
    required this.nameLocal,
    required this.type,
    required this.defaultValue,
    this.min,
    this.max,
    this.enumValues,
    this.useIncrement = false,
    this.dependsOn,
  });

  /// Create parameter from JSON (from ESP32 API)
  factory EffectParameter.fromJson(Map<String, dynamic> json) {
    return EffectParameter(
      id: json['id'] as String,
      name: json['name'] as String,
      nameLocal: json['nameLocal'] as String? ?? json['name'] as String,
      type: ParameterType.values.firstWhere(
        (e) => e.name == json['type'],
        orElse: () => ParameterType.uint8,
      ),
      defaultValue: json['default'],
      min: json['min'] as int?,
      max: json['max'] as int?,
      enumValues: (json['enumValues'] as List<dynamic>?)?.cast<String>(),
    );
  }
}

/// Effect definition
class Effect {
  final int id;
  final String name;
  final String nameLocal;
  final EffectCategory category;
  final String icon;
  final List<EffectParameter> parameters;

  const Effect({
    required this.id,
    required this.name,
    required this.nameLocal,
    required this.category,
    required this.icon,
    required this.parameters,
  });

  /// Create effect from JSON (from ESP32 API)
  factory Effect.fromJson(Map<String, dynamic> json) {
    return Effect(
      id: json['id'] as int,
      name: json['name'] as String,
      nameLocal: json['nameLocal'] as String? ?? json['name'] as String,
      category: EffectCategory.values.firstWhere(
        (e) => e.id == json['category'],
        orElse: () => EffectCategory.static_,
      ),
      icon: json['icon'] as String? ?? 'auto_awesome',
      parameters:
          (json['parameters'] as List<dynamic>?)
              ?.map((p) => EffectParameter.fromJson(p as Map<String, dynamic>))
              .toList() ??
          [],
    );
  }
}

/// Current LED state
class LEDStatus {
  final bool power;
  final int brightness;
  final int effectId;
  final String effectName;
  final int category;
  final int numEffects;

  const LEDStatus({
    required this.power,
    required this.brightness,
    required this.effectId,
    required this.effectName,
    required this.category,
    required this.numEffects,
  });

  factory LEDStatus.fromJson(Map<String, dynamic> json) {
    return LEDStatus(
      power: json['power'] as bool? ?? false,
      brightness: json['brightness'] as int? ?? 180,
      effectId: json['effect'] as int? ?? 0,
      effectName: json['effectName'] as String? ?? 'Unknown',
      category: json['category'] as int? ?? 1,
      numEffects: json['numEffects'] as int? ?? 42,
    );
  }
}
