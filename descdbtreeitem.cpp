#include "DescDbTreeItem.h"

DescDbTreeItem::DescDbTreeItem(QTreeWidgetItem *parent):QTreeWidgetItem(parent), m_itemType(SIMPLE_TEXT), m_active(false)
{

}

void DescDbTreeItem::setType(DescDbTreeItem::TypeItemTree type)
{
    m_itemType = type;
    changeIcon();
}

void DescDbTreeItem::setEditorText(const QString &editorText)
{
    m_editorText = editorText;
}

DescDbTreeItem::TypeItemTree DescDbTreeItem::itemType() const
{
    return m_itemType;
}

QString DescDbTreeItem::editorText() const
{
    return m_editorText;
}

void DescDbTreeItem::changeActive(bool active)
{
    if (m_active != active) {
        m_active = active;
        changeIcon();
    }
}

void DescDbTreeItem::changeIcon()
{
    QIcon icon;
    switch (m_itemType) {
    case TypeItemTree::PROJECT_NAME:
        icon = m_active ? QIcon(":/image/power-button.png") : QIcon(":/image/power.png");
        break;
    case TypeItemTree::SORT_FILTER_NAME: case TypeItemTree::HIGHLIGHT_FILTER_NAME:
        icon = m_active ? QIcon(":/image/power-button.png") : QIcon();
        break;
    case TypeItemTree::SORT_FILTER_FOLDER:
        icon = QIcon(":/image/sort.png");
        break;
    case TypeItemTree::HIGHLIGHT_FILTER_FOLDER:
        icon =  QIcon(":/image/rgb.png");
        break;
    default:
        break;
    }
    setIcon(0, icon);
}
