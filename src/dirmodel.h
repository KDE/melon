#pragma once

#include <KDirModel>
#include <QQmlEngine>
#include <optional>

class SDirModel : public KDirModel
{
	Q_OBJECT
	QML_NAMED_ELEMENT(DirModel)
	QML_UNCREATABLE("only c++ is allowed to make new dir models")

    struct Private;
    QScopedPointer<Private> d;

    std::optional<QPointF> getXAttr(const KFileItem& item) const;
    void setXAttr(const KFileItem& item, const QPointF& pos) const;

public:
	explicit SDirModel(QObject* parent = nullptr);
	~SDirModel();

    enum AdditionalRoles {
        // Note: use   printf "0x%08X\n" $(($RANDOM*$RANDOM))
        // to define additional roles.
        IsImageReadable = 0x1B445A2C, ///< returns the KFileItem for a given index. roleName is "fileItem".
        IconPosition = 0x1C94B88F,
    };

    QPointF positionFor(const KFileItem& item) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
};
