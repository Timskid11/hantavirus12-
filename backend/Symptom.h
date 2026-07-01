#ifndef SYMPTOM_H
#define SYMPTOM_H

#include <string>

// Which phase of Hantavirus Pulmonary Syndrome a symptom belongs to.
// Early-phase symptoms are non-specific (flu-like); late-phase symptoms
// signal progression toward cardiopulmonary involvement and are far
// more clinically urgent.
enum class Phase {
    Early,
    Late
};

class Symptom {
public:
    Symptom(std::string id, std::string label, Phase phase, int weight)
        : id_(std::move(id)), label_(std::move(label)), phase_(phase), weight_(weight) {}

    const std::string& id() const { return id_; }
    const std::string& label() const { return label_; }
    Phase phase() const { return phase_; }
    int weight() const { return weight_; }

private:
    std::string id_;
    std::string label_;
    Phase phase_;
    int weight_; // contribution to risk score if present
};

#endif // SYMPTOM_H
