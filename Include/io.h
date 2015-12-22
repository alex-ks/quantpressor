#pragma once

/*
	Заголовочный файл, определяющий формат взаимодействия модуля с внешним миром.
	Простые Rough-реализации интерфейсов, объявленных здесь, можно найти в проекте 
*/

#include <string>
#include <vector>

#include "memory.h"

namespace module_api
{
	typedef unsigned int uint;

	/*
		Исключения, выбрасываемые методом при возникновении ошибок
		Список нужно дополнять при необходимости
	*/
	enum class InputException
	{
		INDEX_OUT_OF_RANGE,
		REVERSED_RECTANGLE,				//устаревшее и не использующееся
		UNKNOWN, 
		BAD_INPUT						//неверные аргументы метода
	};

	enum class ReplacementState
	{
		REPLACES_ROWS,					//замещение для строк
		REPLACES_COLUMNS,				//замещение для столбцов
		ABSENT							//замещение отсутствует
	};

	///<summary>
	///Интерфейс, предоставляющий доступ к информации о содержимом таблицы
	///</summary>
	class IMetadata
	{
	public:

		///<summary>
		///<para>Replacements - замена качественных значений столбцов на цифровые.</para>
		///<para>Возвращает replacement для данного столбца.</para>
		///</summary>
		///<param name="index">Номер требуемого столбца.</param>
		///<returns>Вектор строк, индексы которого - числовые коды строк в данном столбце.</returns>
		virtual const std::vector<std::wstring> &get_replacement( uint index ) const = 0;

		///<summary>
		///<para>Сообщает, происходит ли замена значений в строке, в столбце, или вовсе отсутствует.</para>
		///</summary>
		///<returns>Текущее состояние replacement'ов</returns>
		virtual ReplacementState get_replacements_state( ) const = 0;

		///<summary>
		///<para>Возвращает имя столбца с данным номером или пустую строку, если имени нет.</para>
		///</summary>
		///<param name="column">Номер требуемого столбца.</param>
		///<returns>Имя стобца или пустая строка.</returns>
		virtual std::wstring get_column_name( uint column ) const = 0;

		///<summary>
		///<para>Возвращает вектор имен столбцов или пустой вектор, если таковых нет.</para>
		///</summary>
		///<returns>Вектор с именами или пустой вектор.</returns>
		virtual const std::vector<std::wstring> &get_column_names( ) const = 0;

		///<summary>
		///<para>Возвращает количество столбцов в таблице.</para>
		///</summary>
		///<returns>Количество столбцов в таблице.</returns>
		virtual uint get_column_count( ) const = 0;
		
		///<summary>
		///<para>Устанавливает имя для требуемого столбца.</para>
		///</summary>
		///<param name="column">Номер требуемого столбца</param>
		///<param name="new_name">Новое имя</param>
		virtual void set_column_name( uint column, const std::wstring &new_name ) = 0;

		///<summary>
		///<para>Устанавливает имена для столбцов.</para>
		///</summary>
		///<param name="names">Вектор строк-имен. Его длина должна быть равна количесту столбцов.</param>
		virtual void set_column_names( const std::vector<std::wstring> &names ) = 0;

		///<summary>
		///<para>Возвращает имя строки с данным номером или пустую строку, если имени нет.</para>
		///</summary>
		///<param name="row">Номер требуемой строки.</param>
		///<returns>Имя строки или пустая строка.</returns>
		virtual std::wstring get_row_name( uint row ) const = 0;
		
		///<summary>
		///<para>Возвращает вектор имен строк или пустой вектор, если таковых нет.</para>
		///</summary>
		///<returns>Вектор с именами или пустой вектор.</returns>
		virtual const std::vector<std::wstring> &get_row_names( ) const = 0;

		///<summary>
		///<para>Возвращает количество строк в таблице.</para>
		///</summary>
		///<returns>Количество строк в таблице.</returns>
		virtual uint get_row_count( ) const = 0;

		///<summary>
		///<para>Устанавливает имя для требуемой строки.</para>
		///</summary>
		///<param name="row">Номер требуемой строки</param>
		///<param name="new_name">Новое имя</param>
		virtual void set_row_name( uint row, const std::wstring &new_name ) = 0;

		///<summary>
		///<para>Устанавливает имена для строк.</para>
		///</summary>
		///<param name="names">Вектор строк-имен. Его длина должна быть равна количесту столбцов.</param>
		virtual void set_row_names( const std::vector<std::wstring> &names ) = 0;

		///<summary>
		///<para>Деструктор с пустым телом для обеспечения виртуального полиморфизма.</para>
		///</summary>
		virtual ~IMetadata( ) { }
	};

	typedef HeapPtr<IMetadata> pIMetadata;

	typedef HeapPtr<double[]> DoubleArray;

	///<summary>
	///Интерфейс, обеспечивающий доступ к данным в виде таблицы
	///</summary>
	class IGrid
	{
	public:

		///<summary>
		///<para>Возвращает значение ячейки таблицы.</para>
		///</summary>
		///<param name="row">Номер строки ячейки.</param>
		///<param name="column">Номер столбца ячейки.</param>
		///<returns>Численное значение ячейки.</returns>
		virtual double get_value( uint row, uint column ) const = 0;

		///<summary>
		///<para>Возвращает двумерный массив-подтаблицу в виде непрерывной области памяти.</para>
		///</summary>
		///<param name="min_row">Номер строки начальной ячейки.</param>
		///<param name="min_column">Номер столбца начальной ячейки.</param>
		///<param name="row_count">Количество строк подтаблицы.</param>
		///<param name="column_count">Количество столбцов подтаблицы.</param>
		///<returns>Указатель на массив.</returns>
		virtual DoubleArray get_rect( uint min_row, uint min_column, uint row_count, uint column_count ) const = 0;

		///<summary>
		///<para>Возвращает количество столбцов в таблице.</para>
		///</summary>
		///<returns>Количество столбцов.</returns>
		virtual uint get_column_count( ) const = 0;

		///<summary>
		///<para>Возвращает количество строк в таблице.</para>
		///</summary>
		///<returns>Количество строк.</returns>
		virtual uint get_row_count( ) const = 0;

		/*
			Возвращает указатель на объект IMetadata, содержащий информацию о таблице
		*/

		///<summary>
		///<para>Возвращает указатель на объект IMetadata, содержащий информацию о таблице.</para>
		///</summary>
		///<returns>Указатель на IMetadata.</returns>
		virtual pIMetadata get_metadata( ) const = 0;

		///<summary>
		///<para>Деструктор с пустым телом для обеспечения виртуального полиморфизма.</para>
		///</summary>
		virtual ~IGrid( ) { }
	};

	typedef HeapPtr<IGrid> pIGrid;

	///<summary>
	///Интерфейс контейнера, содержащего входные параметры для модуля
	///</summary>
	class IArguments
	{
	public:
		/*
			Возвращает одну из таблиц, пронумерованных от 0 до get_grid_count( ) - 1
		*/

		///<summary>
		///<para>Возвращает требуемую таблицу.</para>
		///</summary>
		///<param name="number">Номер требуемой таблицы.</param>
		///<returns>Указатель на таблицу.</returns>
		virtual pIGrid get_grid( uint number ) const = 0;

		///<summary>
		///<para>Возвращает количество таблиц в контейнере.</para>
		///</summary>
		///<returns>Количество таблиц.</returns>
		virtual uint get_grid_count( ) const = 0;

		/*
			Возвращает флаг - необходимо ли делать анализ результата
			( в т.ч.текстовое описание )
		*/

		///<summary>
		///<para>Возвращает флаг - необходимо ли делать текстовый анализ результата.</para>
		///</summary>
		///<returns>Булево значение: необходимо ли делать текстовый анализ результата.</returns>
		virtual bool make_analytics( ) const = 0;

		///<summary>
		///<para>Деструктор с пустым телом для обеспечения виртуального полиморфизма.</para>
		///</summary>
		virtual ~IArguments( ) { }
	};

	typedef HeapPtr<IArguments> pIArguments;

	typedef std::vector<std::wstring> Paragraphs;

	///<summary>
	///Интерфейс контейнера, содержащего результат работы метода
	///</summary>
	class IResult
	{
	public:
		///<summary>
		///<para>Возвращает требуемую таблицу-результат.</para>
		///</summary>
		///<param name="i">Номер требуемой таблицы.</param>
		///<returns>Указатель на таблицу.</returns>
		virtual pIGrid get_grid( uint i ) const = 0;

		///<summary>
		///<para>Возвращает количество таблиц в контейнере.</para>
		///</summary>
		///<returns>Количество таблиц.</returns>
		virtual uint get_grids_count( ) const = 0;

		///<summary>
		///<para>Возвращает вектор строк-абзацев - текстовое описание результата работы модуля.</para>
		///</summary>
		///<returns>Вектор строк-абзацев.</returns>
		virtual Paragraphs get_plain_text( ) const = 0;

		///<summary>
		///<para>Деструктор с пустым телом для обеспечения виртуального полиморфизма.</para>
		///</summary>
		virtual ~IResult( ) { }
	};

	typedef HeapPtr<IResult> pIResult;

	//Для упрощения сборки текста

	template<typename T> inline std::wstring operator+( const std::wstring &str, const T &value )
	{
		return str + ( std::to_wstring( value ) );
	}

	template<typename T> inline std::wstring operator+( const T &value, const std::wstring &str )
	{
		return std::to_wstring( value ) + ( str );
	}
}