/****************************************************************************
** Meta object code from reading C++ file 'HeaderWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/ui/HeaderWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'HeaderWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_HeaderWidget_t {
    QByteArrayData data[11];
    char stringdata0[121];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_HeaderWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_HeaderWidget_t qt_meta_stringdata_HeaderWidget = {
    {
QT_MOC_LITERAL(0, 0, 12), // "HeaderWidget"
QT_MOC_LITERAL(1, 13, 10), // "pinToggled"
QT_MOC_LITERAL(2, 24, 0), // ""
QT_MOC_LITERAL(3, 25, 2), // "on"
QT_MOC_LITERAL(4, 28, 17), // "minimizeRequested"
QT_MOC_LITERAL(5, 46, 14), // "closeRequested"
QT_MOC_LITERAL(6, 61, 13), // "ballRequested"
QT_MOC_LITERAL(7, 75, 14), // "opacityChanged"
QT_MOC_LITERAL(8, 90, 7), // "percent"
QT_MOC_LITERAL(9, 98, 15), // "languageChanged"
QT_MOC_LITERAL(10, 114, 6) // "locale"

    },
    "HeaderWidget\0pinToggled\0\0on\0"
    "minimizeRequested\0closeRequested\0"
    "ballRequested\0opacityChanged\0percent\0"
    "languageChanged\0locale"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_HeaderWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x06 /* Public */,
       4,    0,   47,    2, 0x06 /* Public */,
       5,    0,   48,    2, 0x06 /* Public */,
       6,    0,   49,    2, 0x06 /* Public */,
       7,    1,   50,    2, 0x06 /* Public */,
       9,    1,   53,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void, QMetaType::QString,   10,

       0        // eod
};

void HeaderWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<HeaderWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->pinToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->minimizeRequested(); break;
        case 2: _t->closeRequested(); break;
        case 3: _t->ballRequested(); break;
        case 4: _t->opacityChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->languageChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (HeaderWidget::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&HeaderWidget::pinToggled)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (HeaderWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&HeaderWidget::minimizeRequested)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (HeaderWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&HeaderWidget::closeRequested)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (HeaderWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&HeaderWidget::ballRequested)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (HeaderWidget::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&HeaderWidget::opacityChanged)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (HeaderWidget::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&HeaderWidget::languageChanged)) {
                *result = 5;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject HeaderWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_HeaderWidget.data,
    qt_meta_data_HeaderWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *HeaderWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *HeaderWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_HeaderWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int HeaderWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void HeaderWidget::pinToggled(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void HeaderWidget::minimizeRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void HeaderWidget::closeRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void HeaderWidget::ballRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void HeaderWidget::opacityChanged(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void HeaderWidget::languageChanged(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
