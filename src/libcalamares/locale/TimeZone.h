/* === This file is part of Calamares - <https://github.com/calamares> ===
 *
 *   SPDX-FileCopyrightText: 2019 Adriaan de Groot <groot@kde.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 *   Calamares is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Calamares is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Calamares. If not, see <http://www.gnu.org/licenses/>.
 *
 *
 */

#ifndef LOCALE_TIMEZONE_H
#define LOCALE_TIMEZONE_H

#include "DllMacro.h"

#include "locale/TranslatableString.h"

#include <QAbstractListModel>
#include <QObject>
#include <QSortFilterProxyModel>
#include <QVariant>

namespace CalamaresUtils
{
namespace Locale
{
class Private;
class RegionalZonesModel;
class ZonesModel;

class TimeZoneData : public QObject, TranslatableString
{
    friend class RegionalZonesModel;
    friend class ZonesModel;

    Q_OBJECT

    Q_PROPERTY( QString region READ region CONSTANT )

public:
    TimeZoneData( const QString& region,
                  const QString& zone,
                  const QString& country,
                  double latitude,
                  double longitude );
    TimeZoneData( const TimeZoneData& ) = delete;
    TimeZoneData( TimeZoneData&& ) = delete;

    QString tr() const override;

    QString region() const { return m_region; }
    QString zone() const { return key(); }

    QString country() const { return m_country; }
    double latitude() const { return m_latitude; }
    double longitude() const { return m_longitude; }

private:
    QString m_region;
    QString m_country;
    double m_latitude;
    double m_longitude;
};


/** @brief The list of timezone regions
 *
 * The regions are a short list of global areas (Africa, America, India ..)
 * which contain zones.
 */
class DLLEXPORT RegionsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles
    {
        NameRole = Qt::DisplayRole,
        KeyRole = Qt::UserRole  // So that currentData() will get the key
    };

    RegionsModel( QObject* parent = nullptr );
    virtual ~RegionsModel() override;

    int rowCount( const QModelIndex& parent ) const override;
    QVariant data( const QModelIndex& index, int role ) const override;

    QHash< int, QByteArray > roleNames() const override;

private:
    Private* m_private;
};

class DLLEXPORT ZonesModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles
    {
        NameRole = Qt::DisplayRole,
        KeyRole = Qt::UserRole,  // So that currentData() will get the key
        RegionRole = Qt::UserRole + 1
    };

    ZonesModel( QObject* parent = nullptr );
    virtual ~ZonesModel() override;

    int rowCount( const QModelIndex& parent ) const override;
    QVariant data( const QModelIndex& index, int role ) const override;

    QHash< int, QByteArray > roleNames() const override;

    /** @brief Iterator for the underlying list of zones
     *
     * Iterates over all the zones in the model. Operator * may return
     * a @c nullptr when the iterator is not valid. Typical usage:
     *
     * ```
     * for( auto it = model.begin(); it; ++it )
     * {
     *     const auto* zonedata = *it;
     *     ...
     * }
     */
    class Iterator
    {
        friend class ZonesModel;
        Iterator( const Private* m )
            : m_index( 0 )
            , m_p( m )
        {
        }

    public:
        operator bool() const;
        void operator++() { ++m_index; }
        const TimeZoneData* operator*() const;
        int index() const { return m_index; }

    private:
        int m_index;
        const Private* m_p;
    };

    Iterator begin() const { return Iterator( m_private ); }

public Q_SLOTS:
    /** @brief Look up TZ data based on its name.
     *
     * Returns @c nullptr if not found.
     */
    const TimeZoneData* find( const QString& region, const QString& zone ) const;

    /** @brief Look up TZ data based on the location.
     *
     * Returns the nearest zone to the given lat and lon.
     */
    const TimeZoneData* find( double latitude, double longitude ) const;

    /** @brief Look up TZ data based on the location.
     *
     * Returns the nearest zone, or New York. This is non-const for QML
     * purposes, but the object should be considered const anyway.
     */
    QObject* lookup( double latitude, double longitude ) const;

private:
    Private* m_private;
};

class DLLEXPORT RegionalZonesModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY( QString region READ region WRITE setRegion NOTIFY regionChanged )

public:
    RegionalZonesModel( ZonesModel* source, QObject* parent = nullptr );
    ~RegionalZonesModel() override;

    bool filterAcceptsRow( int sourceRow, const QModelIndex& sourceParent ) const override;

    QString region() const { return m_region; }

public Q_SLOTS:
    void setRegion( const QString& r );

signals:
    void regionChanged( const QString& );

private:
    Private* m_private;
    QString m_region;
};


}  // namespace Locale
}  // namespace CalamaresUtils

#endif  // LOCALE_TIMEZONE_H
