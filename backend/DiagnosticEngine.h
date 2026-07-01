#ifndef DIAGNOSTIC_ENGINE_H
#define DIAGNOSTIC_ENGINE_H

#include <vector>
#include "Symptom.h"
#include "Patient.h"
#include "DiagnosisResult.h"

// DiagnosticEngine encapsulates the clinical scoring rules used to
// estimate Hantavirus Pulmonary Syndrome (HPS) risk from a patient's
// reported symptoms and exposure history. It owns the canonical
// symptom catalog and exposes it so the frontend can render a form
// without duplicating clinical knowledge on the client side.
class DiagnosticEngine {
public:
    DiagnosticEngine();

    const std::vector<Symptom>& catalog() const { return catalog_; }

    DiagnosisResult evaluate(const Patient& patient) const;

private:
    std::vector<Symptom> catalog_;

    const Symptom* find(const std::string& id) const;
};

#endif // DIAGNOSTIC_ENGINE_H
