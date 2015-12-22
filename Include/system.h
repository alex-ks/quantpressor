#pragma once
#include <map>
#include <string>

#include "notifications.h"
#include "exception.h"
#include "memory.h"
#include "io.h"

namespace module_api
{
	typedef std::map<std::wstring, std::wstring> Resources;

	///<summary>
	///Интерфейс, содержащий callbacks для получения результата асинхронно запущенного метода.
	///</summary>
	class IResultReceiver
	{
	public:
		///<summary>
		///<para>В эту функцию будет передан результат работы запрошенного метода. Вызывается асинхронно из другого потока.</para>
		///</summary>
		///<remarks>
		///<para>Не рекомендуется продолжать работу в этом потоке.</para>
		///<para>Лучший вариант - каким-либо образом оповестить основной поток о получении результата и завершиться.</para>
		///</remarks>
		virtual void receive_result( const pIResult &result ) = 0;

		///<summary>
		///<para>Эта функция будет вызвана вместо receive_result, если вызываеый метод выбросил исключение. Так же вызывается асинхронно из другого потока.</para>
		///</summary>
		///<remarks>
		///<para>Не рекомендуется продолжать работу в этом потоке.</para>
		///<para>Лучший вариант - каким-либо образом оповестить основной поток о получении результата и завершиться.</para>
		///</remarks>
		virtual void catch_exception( const Exception &exception ) = 0;

		virtual ~IResultReceiver( ) { }
	};

	///<summary>
	///Интерфейс, предоставляющий методу API для взаимодействия с системой управления модулями.
	///</summary>
	class ISystem
	{
	public:
		virtual Resources get_module_resources( ) = 0;

		///<summary>
		///<para>Запускает метод асинхронно.</para>
		///<para>Управление из функции возвращается сразу, не дожидаясь окончания исполнения метода.</para>
		///</summary>
		///<param name="method_name">Имя метода, которое было передано менеджеру при подключении требуемого модуля. Может не совпадать с именем .dll.</param>
		///<param name="arguments">Аргументы, которые будут переданы требуемому методу.</param>
		///<param name="notifier">Объект, с помощью которого метод сможет посылать уведомления.</param>
		///<param name="receiver">
		///<para>Объект-получатель результата. При выполнении метода, его результат будет передан в соответствующую функцию ресивера.</para>
		///<para>Если запрашивать несколько модулей одновременно, необходимо создать разные экземпляры объектов IResultReceiver. Тогда не будет неоднозначности с определением соответствия модуль - результат.</para>
		///<returns>Булево значение: был ли запущен метод.</returns>
		///</param>
		virtual bool launch_method_async( const std::wstring &method_name,
										  const pIArguments &arguments,
										  const pINotifier &notifier,
										  IResultReceiver *receiver ) = 0;

		///<summary>
		///<para>Запускает требуемый метод синхронно: управление из функции возвращается после окончания исполнения метода, результат возвращается сразу.</para>
		///</summary>
		///<param name="method_name">Имя метода, которое было передано менеджеру при подключении требуемого модуля. Может не совпадать с именем .dll.</param>
		///<param name="arguments">Аргументы, которые будут переданы требуемому методу.</param>
		///<param name="notifier">Объект, с помощью которого метод сможет посылать уведомления.</param>
		///<returns>Результат исполнения метода.</returns>
		///</param>
		virtual pIResult launch_method( const std::wstring &method_name,
										const pIArguments &arguments,
										const pINotifier &notifier ) = 0;

		virtual ~ISystem( ) { }
	};

	typedef HeapPtr<ISystem> pISystem;
}