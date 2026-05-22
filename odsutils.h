#ifndef ODSUTILS_H
#define ODSUTILS_H

#include <ods/decl.hxx>
#include <ods/ods.hxx>
#include <ods/HAlign.hpp>
#include <ods/VAlign.hpp>
#include <ods/inst/decl.hxx>

#include <QString>

namespace odsutils {
  QString FindFile(const QString &file_name);
  void PrintBorder(ods::Cell *cell, const int row, const int col_index);
  void PrintPercentage(ods::Cell *cell);
  void PrintWidth(ods::inst::TableTableColumn *col, const ods::MustHave mh = ods::MustHave::Yes);
  int ReadFile();
  void Save(ods::Book *book, const QString file_name = nullptr);
  void setValue(ods::Cell* cell, QVariant value);
}

#endif // ODSUTILS_H
