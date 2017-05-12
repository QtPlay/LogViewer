#ifndef DescDbTreeItem_H
#define DescDbTreeItem_H

#include <QDebug>
#include <QObject>
#include <QWidget>
#include <QTreeWidgetItem>

class DescDbTreeItem : public QTreeWidgetItem
{
public:
    enum TypeItemTree {
        PROJECT_NAME,
        SORT_FILTER_NAME,
        HIGHLIGHT_FILTER_NAME,
        SIMPLE_TEXT,
        SORT_FILTER_FOLDER,
        HIGHLIGHT_FILTER_FOLDER
    };

public:
    DescDbTreeItem(QTreeWidgetItem *parent = nullptr);
    void setType(TypeItemTree type);
    void setEditorText(const QString& editorText);
    TypeItemTree itemType() const;
    QString editorText() const;
    void changeActive(bool active);

protected:
    void changeIcon();

private:
    QString m_editorText;
    TypeItemTree m_itemType;
    bool m_active;
};

#endif // DescDbTreeItem_H
