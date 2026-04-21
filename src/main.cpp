#include <QApplication>
#include <QDir>
#include <QTranslator>
#include <QLibraryInfo>

#include "AppConfig.h"
#include "Database.h"
#include "MainWindow.h"
#include "ui/Theme.h"

static void installTranslator(QApplication& app, const QString& localeName)
{
    static QTranslator qtBaseTranslator;
    static QTranslator appTranslator;

    qtBaseTranslator.load("qtbase_" + localeName, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&qtBaseTranslator);

    const auto qmPath = QDir(QCoreApplication::applicationDirPath()).filePath("i18n/MomentWise_" + localeName + ".qm");
    appTranslator.load(qmPath);
    app.installTranslator(&appTranslator);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("MomentWise");
    QApplication::setOrganizationName("MomentWise");

    AppConfig::ensureAppDataDir();
    Database::migrateFromJsonIfNeeded();

    const auto ff = Theme::preferredFontFamily();
    if (!ff.isEmpty()) {
        QFont f(ff);
        f.setStyleStrategy(QFont::PreferAntialias);
        f.setPointSize(16); // 增大默认字体大小
        app.setFont(f);
    } else {
        // 如果没有找到东方字体，使用系统默认字体并增大字号
        QFont f = app.font();
        f.setPointSize(16);
        f.setFamily("Microsoft YaHei UI");
        app.setFont(f);
    }
    app.setStyleSheet(Theme::appStyleSheet());

    const auto locale = AppConfig::uiLocale();
    installTranslator(app, locale);

    MainWindow w;
    w.show();

    return app.exec();
}

