#pragma once

#include <KDirModel>
#include <QQmlEngine>

class SDirModel : public KDirModel
{
	Q_OBJECT
	QML_NAMED_ELEMENT(DirModel)
	QML_UNCREATABLE("only c++ is allowed to make new dir models")

    struct Private;
    QScopedPointer<Private> d;

public:
	explicit SDirModel(QObject* parent = nullptr);
	~SDirModel();

    enum AdditionalRoles {
        // Note: use   printf "0x%08X\n" $(($RANDOM*$RANDOM))
        // to define additional roles.
        IsImageReadable = 0x1B445A2C, ///< returns the KFileItem for a given index. roleName is "fileItem".
    };

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
};
