// Andrew Naplavkov

#include <algorithm>
#include <QComboBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QString>
#include <QStringList>
#include <Qt>
#include <QWidget>
#include <stdexcept>
#include "dialog_insert.h"
#include "layer.h"

dialog_insert::dialog_insert(QWidget* parent, layer_ptr lr_from, layer_ptr lr_to) : QDialog(parent)
{
  setWindowTitle("inserting rows");
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

  if (lr_from->get_levels() != lr_to->get_levels()) throw std::runtime_error("layer error");
  m_tab = new QTabWidget;
  for (size_t lvl(0); lvl < lr_from->get_levels(); ++lvl)
  {
    auto tbl_from(lr_from->get_table_def(lvl));
    auto tbl_to(lr_to->get_table_def(lvl));

    QStringList cols_from;
    for (auto col_from(std::begin(tbl_from.columns)); col_from != std::end(tbl_from.columns); ++col_from)
      if (brig::VoidColumn != col_from->type)
        cols_from.append(QString::fromUtf8(col_from->name.c_str()));
    cols_from.append(QString());

    QGridLayout* grid = new QGridLayout;
    int row(0);
    for (auto col_to(std::begin(tbl_to.columns)); col_to != std::end(tbl_to.columns); ++col_to)
    {
      if (brig::VoidColumn == col_to->type) continue;

      QLabel* lbl(new QLabel);
      lbl->setText(QString::fromUtf8(col_to->name.c_str()));
      grid->addWidget(lbl, row, 0, Qt::AlignRight);

      QComboBox* combo(new QComboBox);
      combo->addItems(cols_from);
      QString name(QString::fromUtf8(col_to->name.c_str()));
      int pos = combo->findText(name, Qt::MatchFixedString | Qt::MatchCaseSensitive);
      if (pos < 0) pos = combo->findText(name, Qt::MatchFixedString);
      if (pos < 0) combo->setCurrentIndex(combo->count() - 1);
      else combo->setCurrentIndex(pos);
      grid->addWidget(combo, row, 1);

      if (brig::Geometry == col_to->type && col_to->epsg <= 0)
      {
        QLineEdit* edit(new QLineEdit);
        edit->setValidator(new QIntValidator(0, INT_MAX, this));
        edit->setToolTip("EPSG");
        edit->setText("4326");
        grid->addWidget(edit, row, 2);
      }

      ++row;
    }

    QWidget* w = new QWidget;
    w->setLayout(grid);
    m_tab->addTab(w, QString("%1").arg(lvl));
  }

  m_ccw = new QCheckBox("make counter-clockwise for exterior rings, and clockwise for interior rings");
  m_ccw->setCheckState(Qt::Unchecked);

  m_view = new QCheckBox("current extent only");
  m_view->setCheckState(Qt::Unchecked);

  QHBoxLayout* buttons(new QHBoxLayout);
  QPushButton* insert_btn(new QPushButton("insert"));
  connect(insert_btn, SIGNAL(clicked()), this, SLOT(accept()));
  buttons->addWidget(insert_btn);
  QPushButton* cancel_btn(new QPushButton("cancel"));
  connect(cancel_btn, SIGNAL(clicked()), this, SLOT(reject()));
  buttons->addWidget(cancel_btn);

  QVBoxLayout* layout(new QVBoxLayout);
  layout->addWidget(m_tab);
  layout->addWidget(m_ccw);
  layout->addWidget(m_view);
  layout->addLayout(buttons);
  setLayout(layout);
}

std::vector<insert_item> dialog_insert::get_items() const
{
  std::vector<insert_item> res;
  for (int level(0); level < m_tab->count(); ++level)
  {
    QGridLayout* grid = static_cast<QGridLayout*>(m_tab->widget(level)->layout());
    for (int row(0); row < grid->rowCount(); ++row)
    {
      insert_item item;
      item.level = level;
      item.column_to = static_cast<QLabel*>(grid->itemAtPosition(row, 0)->widget())->text().toUtf8().constData();
      item.column_from = static_cast<QComboBox*>(grid->itemAtPosition(row, 1)->widget())->currentText().toUtf8().constData();
      if (item.column_from.empty()) continue;
      if (2 < grid->columnCount() && 0 != grid->itemAtPosition(row, 2))
      {
        QString str(static_cast<QLineEdit*>(grid->itemAtPosition(row, 2)->widget())->text());
        if (!str.isEmpty()) item.epsg = str.toInt();
      }
      res.push_back(item);
    }
  }
  return res;
}
