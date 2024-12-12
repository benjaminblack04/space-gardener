#pragma once

// Helper function to ensure something is within a range;
// if it's not, it will be clamped to the nearest value.
#define clamp(value, min, max) (value < min) ? min : (value > max) ? max : value
