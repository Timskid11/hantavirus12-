#include <iostream>
#include "httplib.h"
#include "json.hpp"
#include "DiagnosticEngine.h"
#include "Patient.h"
#include "DiagnosisResult.h"

using json = nlohmann::json;

static std::string phaseToString(Phase p) {
    return p == Phase::Early ? "early" : "late";
}

int main() {
    DiagnosticEngine engine;
    httplib::Server svr;

    // Allow the separately-served frontend (different origin/port) to call this API.
    svr.set_default_headers({
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Methods", "GET, POST, OPTIONS"},
        {"Access-Control-Allow-Headers", "Content-Type"}
    });

    svr.Options(R"(/.*)", [](const httplib::Request&, httplib::Response& res) {
        res.status = 204;
    });

    // GET /api/symptoms -> the symptom catalog, so the frontend form
    // is always in sync with the backend's clinical rules.
    svr.Get("/api/symptoms", [&](const httplib::Request&, httplib::Response& res) {
        json out = json::array();
        for (const auto& s : engine.catalog()) {
            out.push_back({
                {"id", s.id()},
                {"label", s.label()},
                {"phase", phaseToString(s.phase())}
            });
        }
        res.set_content(out.dump(), "application/json");
    });

    // POST /api/diagnose
    // Body: { "symptomIds": ["fever", "coughing", ...],
    //         "rodentExposure": bool,
    //         "ruralOrEnclosedSpaceExposure": bool,
    //         "recentCleaningOfEnclosedSpace": bool }
    svr.Post("/api/diagnose", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            json body = json::parse(req.body);
            Patient patient;

            if (body.contains("symptomIds") && body["symptomIds"].is_array()) {
                for (const auto& id : body["symptomIds"]) {
                    if (id.is_string()) patient.addSymptomId(id.get<std::string>());
                }
            }
            if (body.contains("rodentExposure") && body["rodentExposure"].is_boolean()) {
                patient.setRodentExposure(body["rodentExposure"].get<bool>());
            }
            if (body.contains("ruralOrEnclosedSpaceExposure") && body["ruralOrEnclosedSpaceExposure"].is_boolean()) {
                patient.setRuralOrEnclosedSpaceExposure(body["ruralOrEnclosedSpaceExposure"].get<bool>());
            }
            if (body.contains("recentCleaningOfEnclosedSpace") && body["recentCleaningOfEnclosedSpace"].is_boolean()) {
                patient.setRecentCleaningOfEnclosedSpace(body["recentCleaningOfEnclosedSpace"].get<bool>());
            }

            DiagnosisResult result = engine.evaluate(patient);

            json out = {
                {"score", result.score},
                {"level", riskLevelToString(result.level)},
                {"lateStagePresent", result.lateStagePresent},
                {"exposureConfirmed", result.exposureConfirmed},
                {"recommendation", result.recommendation},
                {"flaggedSymptoms", result.flaggedSymptoms}
            };
            res.set_content(out.dump(), "application/json");
        } catch (const std::exception& e) {
            res.status = 400;
            json err = {{"error", std::string("Invalid request: ") + e.what()}};
            res.set_content(err.dump(), "application/json");
        }
    });

    svr.Get("/api/health", [](const httplib::Request&, httplib::Response& res) {
        res.set_content(R"({"status":"ok"})", "application/json");
    });

    // Render (and most PaaS hosts) assign the port dynamically via $PORT.
    // Fall back to 8080 for local runs.
    int port = 8080;
    if (const char* envPort = std::getenv("PORT")) {
        port = std::atoi(envPort);
    }

    std::cout << "Hantavirus diagnostic API listening on port " << port << "\n";
    svr.listen("0.0.0.0", port);
    return 0;
}
