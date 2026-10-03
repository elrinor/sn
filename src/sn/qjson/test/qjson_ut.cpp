#include <cmath>

#include <gtest/gtest.h>

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

TEST(qjson, qt_invalid_fp) {
    // We pin down the way Qt handles NAN and INFINITY in this test. Not a test for sn.
    auto make_qt_json_doc_text = [] (double value) {
        QJsonObject object;
        object[QStringLiteral()] = value;
        return QJsonDocument(object).toJson(QJsonDocument::Compact).toStdString();
    };

    EXPECT_EQ(make_qt_json_doc_text(0), "{\"\":0}");
    EXPECT_EQ(make_qt_json_doc_text(HUGE_VAL), "{\"\":null}");
    EXPECT_EQ(make_qt_json_doc_text(-HUGE_VAL), "{\"\":null}");
    EXPECT_EQ(make_qt_json_doc_text(-NAN), "{\"\":null}");

    // Given what we see above, Qt obviously can't round-trip NAN and INFINITY, toDouble() just returns a
    // default-constructed double.
    EXPECT_EQ(QJsonDocument::fromJson("{\"\":null}").object()[QStringLiteral()].toDouble(), 0.0);
}
