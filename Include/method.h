#pragma once

#include "memory.h"
#include "system.h"
#include "Notifications.h"

#ifdef DLL_EXPORT
#define DLL_API __declspec( dllexport )
#else
#define DLL_API __declspec( dllimport )
#endif

namespace module_api
{
	class IMethod
	{
	public:
		/*
		На вход методу подается объект-контейнер с интерфейсом IArguments,
		содержащий объекты IGrid и флаг

		Время жизни входных данных полностью управляется менеджером, в частности, модуль не должен освобождать память из под таблиц

		Возвращаемое значение - указатель на контейнер, реализующий интерфейс IResult. Его будет использовать менеджер для дальнейшей работы с результатом.
		*/

		///<summary>
		///<para>Тело метода. Вызывается менеджером задач при запуске метода. Возвращает контейнер с результатами работы.</para>
		///</summary>
		///<param name="arguments">Аргументы для вызываемого метода.</param>
		///<param name="notifier">Объект, с помощью которого метод сможет посылать уведомления.</param>
		///<returns>Результат исполнения метода.</returns>
		///</param>
		virtual pIResult run( const pIArguments &arguments, const pINotifier &notifier ) = 0;

		///<summary>
		///<para>Деструктор с пустым телом для обеспечения виртуального полиморфизма.</para>
		///</summary>
		virtual ~IMethod( ) { }
	};

	typedef HeapPtr<IMethod> pIMethod;

	///<summary>
	///<para>Точка входа в .dll - именно эта функция будет вызываться менеджером для запуска модуля.</para>
	///<para>Чтобы эта функция корректно экспортировалась, необходимо заполнить .def файл (см. export.def в проекте).</para>
	///<para>Создает экземпляр объекта-метода, выполняющего требуемые вычисления.</para>
	///<para>Один объект - одна сессия вычислений.</para>
	///</summary>
	DLL_API pIMethod create_instance( const module_api::pISystem & );
}