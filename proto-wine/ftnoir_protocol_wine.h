#pragma once

#include "api/plugin-api.hpp"
#include "compat/shm.h"
#include "wine-shm.h"

#include "ui_ftnoir_winecontrols.h"

#include "options/options.hpp"
using namespace options;

#include <QString>
#include <QProcess>
#include <QMutex>

#include <QDebug>

struct settings : opts
{
    settings() : opts{"proto-wine"} {}
    value<bool> variant_proton{b, "variant-proton", false },
                variant_wine{b, "variant-wine", true };
    value<QString> proton_version{b, "proton-version", {} };
};

class wine : TR, public IProtocol
{
    Q_OBJECT

public:
    wine();
    ~wine() override;

    module_status initialize() override;
    void pose(const double* headpose, const double*) override;

    QString game_name() override
    {
#ifndef OTR_WINE_NO_WRAPPER
        QMutexLocker foo(&game_name_mutex);
        return connected_game;
#else
        return QStringLiteral("X-Plane");
#endif
    }
private:
    shm_wrapper lck_shm { WINE_SHM_NAME, WINE_MTX_NAME, sizeof(WineSHM) };
    WineSHM* shm = nullptr;
    settings s;

#ifndef OTR_WINE_NO_WRAPPER
    QProcess wrapper;
    int gameid = 0;
    QString connected_game;
    QMutex game_name_mutex;
#endif
};

class FTControls: public IProtocolDialog
{
    Q_OBJECT

public:
    FTControls();
    void register_protocol(IProtocol *) override {}
    void unregister_protocol() override {}

private:
    Ui::UICFTControls ui;
    settings s;

private slots:
    void doOK();
    void doCancel();
};

class wine_metadata : public Metadata
{
    Q_OBJECT

public:
#ifndef OTR_WINE_NO_WRAPPER
    QString name() override { return tr("Wine -- Windows layer for Unix"); }
    QIcon icon() override { return QIcon(":/images/wine.png"); }
#else
    QString name() override { return tr("X-Plane"); }
    QIcon icon() override { return {}; }
#endif
};
