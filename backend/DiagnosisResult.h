#ifndef DIAGNOSIS_RESULT_H
#define DIAGNOSIS_RESULT_H

#include <string>
#include <vector>

enum class RiskLevel {
    Low,
    Moderate,
    High,
    Critical
};

inline std::string riskLevelToString(RiskLevel level) {
    switch (level) {
        case RiskLevel::Low: return "LOW";
        case RiskLevel::Moderate: return "MODERATE";
        case RiskLevel::High: return "HIGH";
        case RiskLevel::Critical: return "CRITICAL";
    }
    return "UNKNOWN";
}

class DiagnosisResult {
public:
    DiagnosisResult() = default;

    int score = 0;
    RiskLevel level = RiskLevel::Low;
    bool lateStagePresent = false;
    bool exposureConfirmed = false;
    std::string recommendation;
    std::vector<std::string> flaggedSymptoms;
};

#endif // DIAGNOSIS_RESULT_H
