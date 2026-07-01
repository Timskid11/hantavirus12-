#ifndef PATIENT_H
#define PATIENT_H

#include <string>
#include <vector>
#include <algorithm>

class Patient {
public:
    void addSymptomId(const std::string& id) {
        if (std::find(symptomIds_.begin(), symptomIds_.end(), id) == symptomIds_.end()) {
            symptomIds_.push_back(id);
        }
    }

    void setRodentExposure(bool v) { rodentExposure_ = v; }
    void setRuralOrEnclosedSpaceExposure(bool v) { ruralExposure_ = v; }
    void setRecentCleaningOfEnclosedSpace(bool v) { cleaningExposure_ = v; }

    const std::vector<std::string>& symptomIds() const { return symptomIds_; }
    bool rodentExposure() const { return rodentExposure_; }
    bool ruralOrEnclosedSpaceExposure() const { return ruralExposure_; }
    bool recentCleaningOfEnclosedSpace() const { return cleaningExposure_; }

    bool hasSymptom(const std::string& id) const {
        return std::find(symptomIds_.begin(), symptomIds_.end(), id) != symptomIds_.end();
    }

private:
    std::vector<std::string> symptomIds_;
    bool rodentExposure_ = false;
    bool ruralExposure_ = false;
    bool cleaningExposure_ = false;
};

#endif // PATIENT_H
