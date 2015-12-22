#pragma once

#include <mutex>
#include <atomic>
#include <condition_variable>

#include "exception.h"
#include "system.h"
#include "memory.h"
#include "io.h"

namespace module_api
{
	///<summary>
	///<para>Класс для облегчения работы с асинхронными вызовами методов.</para>
	///<para>Позволяет проверять готовность результата, а так же ждать результат до его получения.</para>
	///</summary>
	class DelayedReceiver : public IResultReceiver
	{
	private:
		volatile std::atomic<bool> ready;
		std::mutex mutex;
		std::condition_variable var_to_wait_on;

		HeapPtr<Exception> exception_ptr;
		pIResult result;

	public:
		DelayedReceiver( );
		~DelayedReceiver( );

		virtual void receive_result( const pIResult & result ) override;
		virtual void catch_exception( const Exception & exception ) override;

		///<summary>
		///<para>Возвращает результат работы метода, если он уже готов.</para>
		///<para>Если результат не готов, блокируется до тех пор, пока кто-либо не сообщит о готовности результата.</para>
		///<para>Если во время исполнения метода было выброшено исключение, то оно выбросится при вызове этой функции.</para>
		///</summary>
		///<returns>Результат работы вызванного метода.</returns>
		pIResult get_result( );

		///<summary>
		///<para>Проверяет готовность результата.</para>
		///</summary>
		///<returns>Булево значение: готовность результата</returns>
		bool is_ready( ) const noexcept;
	};
}