D:/PROJECT/Qt/ACDAnalizerQt/build/Desktop_Qt_6_11_0_MinGW_64_bit-Release
D:/PROJECT/Qt/ACDAnalizer

# Подготовка папки развертывания
создайте новую папку (например, C:\MyProjectDeploy) и скопируйте в нее скомпилированный файл .exe.
# Откройте командную строку Qt
в меню «Пуск» откройте командную строку, соответствующую вашему компилятору (например, Qt 6.x (MSVC 2022 64-бит) или MinGW ). Это гарантирует наличие правильной версии windeployqtи необходимых DLL-файлов в пути среды выполнения.
# Запуск инструмента
Перейдите в папку развертывания и выполните следующую команду:
```bash
windeployqt ACDAnalizer.exe
```

# ods.cc : 335
```cpp
QStringView
TypeToString(const ods::ValueType value_type)
{
	switch (value_type)
	{
	case ods::ValueType::Double: return ns::kDouble;
	case ods::ValueType::Integer: return ns::kInteger;
	case ods::ValueType::String: return ns::kString;
	case ods::ValueType::Currency: return ns::kCurrency;
	case ods::ValueType::Percentage: return ns::kPercentage;
	case ods::ValueType::Date: return ns::kDate;
	case ods::ValueType::DateTime: return **ns::kDate**; //ns::kDateTime;
	case ods::ValueType::Time: return ns::kTime;
	case ods::ValueType::Bool: return ns::kBoolean;
	default: mtl_it_happened(); return QString();
	}
}
```

# Cell.cpp : 808
```cpp
void Cell::WriteValue(QXmlStreamWriter &xml)
{
        if (is_double())
	{
	        Write(xml, ns_->office(), ns::kValue, QString::number(*as_double()));
	} else if (is_integer()) {
	        Write(xml, ns_->office(), ns::kValue, QString::number(*as_integer()));
	} else if (is_currency()) {
	        Write(xml, ns_->office(), ns::kValue, QString::number(*as_double()));
		Write(xml, ns_->office(), ns::kCurrency, office_currency_);
	} else if (is_percentage()) {
	        Write(xml, ns_->office(), ns::kValue, QString::number(*as_double()));
	} else if (is_date()) {
//<table:table-cell table:style-name="ce5" office:value-type="date"
//		office:date-value="1983-12-30">
//<text:p>30.12.1983</text:p>
//</table:table-cell>
                auto *dt = as_date();
		QString date_value = dt->toString(Qt::ISODate);
		Write(xml, ns_->office(), ns::kDateValue, date_value);
	} else if (is_date_time()) {
	        QDateTime *dt = as_date_time();
		QString date_value = dt->toString(**Qt::ISODateWithMs**);
		Write(xml, ns_->office(), ns::kDateValue, date_value);
	} else if (is_time()) {
	        auto *dd = as_time();
		MTL_CHECK_VOID(dd);
		QString dur_value = dd->toString();
		Write(xml, ns_->office(), ns::kTimeValue, dur_value);
	} else if (is_boolean()) {
	        QString str = *as_boolean() ? QLatin1String("true") : QLatin1String("false");
		Write(xml, ns_->office(), ns::kBooleanValue, str);
	} else if (is_string()) {
	        // do nothing
	} else if (!is_value_set()) {
	} else {
	        auto str = QueryAddress();
		auto *sheet = row_->sheet();
		QString s = str + QLatin1String(", sheet name: ") + sheet->name();
		mtl_printq(s);
	}
	
        if (formula_ != nullptr) {
	        Write(xml, ns_->table(), ns::kFormula, formula_->ToXmlString());
	}
}
```

# Ошибка в среде Linux (Ubuntu) по поводу библиотеки osd2->zlib
В файле по пути Desktop_Qt_6_11_1-Debug/_deps/ods2-subbuild/CMakeCache.txt заменить zlib на **ZLIB**

f (USE_SYSTEM_ZLIB)
  find_package(**ZLIB** REQUIRED)
else()
    add_subdirectory(zlib SYSTEM)
endif()

Возможно перед этим нужно установитьь пакет
```bash
sudo apt-get install zlib1g-dev
```

# шибка в среде Linux (Ubuntu) по поводу библиотеки osd2-src/ods/io.cc
```cpp
#include <zstd.h>
```
### Тупо закоментировать


# Git на Linux-е !!! В терминале
```bash
git push origin main
```
Можно попробывать встроить PAT в запрос
git remote set-url origin https://<YOUR_GITHUB_TOKEN>@://github.com


