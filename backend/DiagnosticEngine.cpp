#include "DiagnosticEngine.h"
#include <algorithm>

DiagnosticEngine::DiagnosticEngine() {
    // Early / prodromal phase - non-specific, flu-like (1-2 weeks post exposure)
    catalog_.emplace_back("fatigue", "Fatigue", Phase::Early, 6);
    catalog_.emplace_back("fever", "Fever (over 38\u00b0C)", Phase::Early, 8);
    catalog_.emplace_back("muscle_aches", "Muscle aches, especially thighs/hips/back", Phase::Early, 7);
    catalog_.emplace_back("headache", "Headache", Phase::Early, 5);
    catalog_.emplace_back("dizziness", "Dizziness", Phase::Early, 5);
    catalog_.emplace_back("chills", "Chills", Phase::Early, 5);
    catalog_.emplace_back("abdominal_problems", "Nausea, vomiting, or diarrhea", Phase::Early, 6);

    // Late / cardiopulmonary phase - onset typically 4-10 days after early symptoms
    catalog_.emplace_back("coughing", "Dry coughing", Phase::Late, 18);
    catalog_.emplace_back("shortness_of_breath", "Shortness of breath / rapid breathing", Phase::Late, 25);
    catalog_.emplace_back("tightness_chest", "Tightness or a feeling of fluid in the chest", Phase::Late, 22);
    catalog_.emplace_back("rapid_heartbeat", "Rapid heartbeat", Phase::Late, 15);
}

const Symptom* DiagnosticEngine::find(const std::string& id) const {
    for (const auto& s : catalog_) {
        if (s.id() == id) return &s;
    }
    return nullptr;
}

DiagnosisResult DiagnosticEngine::evaluate(const Patient& patient) const {
    DiagnosisResult result;

    for (const auto& id : patient.symptomIds()) {
        const Symptom* s = find(id);
        if (!s) continue; // ignore unknown ids defensively

        result.score += s->weight();
        result.flaggedSymptoms.push_back(s->label());
        if (s->phase() == Phase::Late) {
            result.lateStagePresent = true;
        }
    }

    int exposureScore = 0;
    if (patient.rodentExposure()) exposureScore += 15;
    if (patient.ruralOrEnclosedSpaceExposure()) exposureScore += 10;
    if (patient.recentCleaningOfEnclosedSpace()) exposureScore += 15;
    result.exposureConfirmed = exposureScore > 0;
    result.score += exposureScore;

    // Base classification purely from cumulative score.
    if (result.score >= 80) {
        result.level = RiskLevel::Critical;
    } else if (result.score >= 50) {
        result.level = RiskLevel::High;
    } else if (result.score >= 20) {
        result.level = RiskLevel::Moderate;
    } else {
        result.level = RiskLevel::Low;
    }

    // Clinical override: late-stage (cardiopulmonary) symptoms combined with
    // a plausible exposure history is an emergency regardless of raw score,
    // since HPS can progress to respiratory failure within hours.
    if (result.lateStagePresent && result.exposureConfirmed && result.level != RiskLevel::Critical) {
        result.level = RiskLevel::Critical;
    }

    switch (result.level) {
        case RiskLevel::Low:
            result.recommendation =
                "Symptoms and exposure history do not strongly suggest Hantavirus. "
                "Monitor for new symptoms and consult a physician if they worsen.";
            break;
        case RiskLevel::Moderate:
            result.recommendation =
                "Some overlap with early-stage Hantavirus symptoms. "
                "Schedule a medical evaluation, especially if you've had rodent exposure.";
            break;
        case RiskLevel::High:
            result.recommendation =
                "Significant symptom and/or exposure overlap with Hantavirus. "
                "Seek medical attention promptly and mention any rodent exposure to your clinician.";
            break;
        case RiskLevel::Critical:
            result.recommendation =
                "Pattern is consistent with progression toward Hantavirus Pulmonary Syndrome. "
                "Seek emergency medical care immediately.";
            break;
    }

    return result;
}
