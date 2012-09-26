// Andrew Naplavkov

#include <QDir>
#include <QGridLayout>
#include <QLabel>
#include <QSettings>
#include <QtGlobal>
#include "dialog_shape.h"
#include "global.h"

dialog_shape::dialog_shape(QWidget* parent)
  : QFileDialog(parent, "copy shapefile", QDir::currentPath(), "shapefiles (*.shp)")
  , m_charset_combo(0)
  , m_epsg_edit(0)
{
  struct pair { const char *first, *second; };
  static const pair Charsets[] = {
  { "ARMSCII-8", "Armenian" },
  { "ASCII", "US-ASCII" },
  { "BIG5", "Chinese/Traditional" },
  { "BIG5-HKSCS", "Chinese/Hong Kong" },
  { "BIG5-HKSCS:1999", "Chinese/Hong Kong" },
  { "BIG5-HKSCS:2001", "Chinese/Hong Kong" },
  { "CP850", "DOS/OEM Western Europe" },
  { "CP862", "DOS/OEM Hebrew" },
  { "CP866", "DOS/OEM Cyrillic" },
  { "CP874", "DOS/OEM Thai" },
  { "CP932", "DOS/OEM Japanese" },
  { "CP936", "DOS/OEM Chinese" },
  { "CP949", "DOS/OEM Korean" },
  { "CP950", "DOS/OEM Chinese/Big5" },
  { "CP1133", "Laotian" },
  { "CP1250", "Windows Central Europe" },
  { "CP1251", "Windows Cyrillic" },
  { "CP1252", "Windows Latin 1" },
  { "CP1253", "Windows Greek" },
  { "CP1254", "Windows Turkish" },
  { "CP1255", "Windows Hebrew" },
  { "CP1256", "Windows Arabic" },
  { "CP1257", "Windows Baltic" },
  { "CP1258", "Windows Vietnamese" },
  { "EUC-CN", "Chinese" },
  { "EUC-JP", "Japanese" },
  { "EUC-KR", "Korean" },
  { "EUC-TW", "Taiwan" },
  { "GB18030", "Chinese/National Standard" },
  { "GBK", "Chinese/Simplified" },
  { "Georgian-Academy", "Georgian" },
  { "Georgian-PS", "Georgian" },
  { "HZ", "Chinese" },
  { "ISO-2022-CN", "Chinese" },
  { "ISO-2022-CN-EXT", "Chinese" },
  { "ISO-2022-JP", "Japanese" },
  { "ISO-2022-JP-1", "Japanese" },
  { "ISO-2022-JP-2", "Japanese" },
  { "ISO-2022-KR", "Korean" },
  { "ISO-8859-1", "Latin-1 Western European" },
  { "ISO-8859-2", "Latin-2 Central European" },
  { "ISO-8859-3", "Latin-3 South European" },
  { "ISO-8859-4", "Latin-4 North European" },
  { "ISO-8859-5", "Latin/Cyrillic" },
  { "ISO-8859-6", "Latin/Arabic" },
  { "ISO-8859-7", "Latin/Greek" },
  { "ISO-8859-8", "Latin/Hebrew" },
  { "ISO-8859-9", "Latin-5 Turkish" },
  { "ISO-8859-10", "Latin-6 Nordic" },
  { "ISO-8859-11", "Latin/Thai" },
  { "ISO-8859-13", "Latin-7 Baltic Rim" },
  { "ISO-8859-14", "Latin-8 Celtic" },
  { "ISO-8859-15", "Latin-9" },
  { "ISO-8859-16", "Latin-10 South-Eastern European" },
  { "JOHAB", "Korean" },
  { "KOI8-R", "Russian" },
  { "KOI8-U", "Ukrainian" },
  { "KOI8-RU", "Belarusian" },
  { "KOI8-T", "Tajik" },
  { "MacArabic", "MAC Arabic" },
  { "MacCentralEurope", "MAC Central Europe" },
  { "MacCroatian", "MAC Croatian" },
  { "MacCyrillic", "MAC Cyrillic" },
  { "MacGreek", "MAC Greek" },
  { "MacHebrew", "MAC Hebrew" },
  { "MacIceland", "MAC Iceland" },
  { "Macintosh", "MAC" },
  { "MacRoman", "MAC European/Western languages" },
  { "MacRomania", "MAC Romania" },
  { "MacThai", "MAC Thai" },
  { "MacTurkish", "MAC Turkish" },
  { "MacUkraine", "MAC Ukraine" },
  { "MuleLao-1", "Laotian" },
  { "PT154", "Kazakh" },
  { "RK1048", "Kazakh" },
  { "SHIFT_JIS", "Japanese" },
  { "TCVN", "Vietnamese" },
  { "TIS-620", "Thai" },
  { "UTF-8", "UNICODE/Universal" },
  { "VISCII", "Vietnamese" }
  }; // Charsets

  QSettings settings(SettingsIni, QSettings::IniFormat);
  setAcceptMode(QFileDialog::AcceptOpen);
  setFileMode(QFileDialog::ExistingFile);
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
  setDirectory(settings.value(QString("%1/%2").arg(SettingsShapefile).arg(SettingsPath), QDir::currentPath()).toString());

  QLabel* charset_lbl = new QLabel("Charset:", this);
  m_charset_combo = new QComboBox(this);
  for (int i(0), size(sizeof(Charsets) / sizeof(pair)); i < size; ++i)
  {
    QString itm;
    itm += Charsets[i].first;
    itm += ", ";
    itm += Charsets[i].second;
    m_charset_combo->addItem(itm, Charsets[i].first);
  }
  const int pos(m_charset_combo->findData(settings.value(QString("%1/%2").arg(SettingsShapefile).arg(SettingsCharset),
#ifdef Q_OS_WIN32
  "CP1252"
#elif Q_OS_MAC
  "MacCentralEurope"
#else
  "UTF-8"
#endif
    ).toString()));
  if (pos >= 0) m_charset_combo->setCurrentIndex(pos);

  QLabel* epsg_lbl = new QLabel("EPSG:", this);
  m_epsg_edit = new QLineEdit(settings.value(QString("%1/%2").arg(SettingsShapefile).arg(SettingsEPSG), "4326").toString(), this);
  m_epsg_edit->setInputMethodHints(Qt::ImhDigitsOnly);

  QGridLayout* layout = (QGridLayout*)this->layout();
  const int rows = layout->rowCount();
  layout->addWidget(charset_lbl, rows, 0);
  layout->addWidget(m_charset_combo, rows, 1);
  layout->addWidget(epsg_lbl, rows + 1, 0);
  layout->addWidget(m_epsg_edit, rows + 1, 1);
}

QString dialog_shape::charset()
{
  return qvariant_cast<QString>(m_charset_combo->itemData(m_charset_combo->currentIndex()));
}

QString dialog_shape::epsg()
{
  return m_epsg_edit->text();
}

void dialog_shape::accept()
{
  QSettings settings(SettingsIni, QSettings::IniFormat);
  settings.setValue(QString("%1/%2").arg(SettingsShapefile).arg(SettingsPath), directory().absolutePath());
  settings.setValue(QString("%1/%2").arg(SettingsShapefile).arg(SettingsCharset), charset());
  settings.setValue(QString("%1/%2").arg(SettingsShapefile).arg(SettingsEPSG), epsg());
  QFileDialog::accept();
}
