// template <typename T>
// T clamp(T value, T min, T max) {
//     if (value < min) return min; // Si la valeur est inférieure au minimum, retourner min
//     if (value > max) return max; // Si la valeur est supérieure au maximum, retourner max
//     return value; // Sinon, retourner la valeur
// }

// Would be better as a macro -- Benjamin

#define clamp(value, min, max) (value < min) ? min : (value > max) ? max : value
