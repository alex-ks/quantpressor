#pragma once
#include <string>

#include "memory.h"

namespace module_api
{
	///<summary>
	///Класс уведомлений, с помощью которых метод может отправлять сообщения пользователю.
	///</summary>
	class Notification
	{
	private:
		std::wstring message, sender;

	public:
		///<summary>
		///<para>Конструирует объект уведомления.</para>
		///</summary>
		///<param name="sender">Имя метода-отправителя.</param>
		///<param name="message">Текст сообщения.</param>
		///</param>
		Notification( const std::wstring &sender, const std::wstring &message );
		~Notification( );

		std::wstring get_message( ) const;
		std::wstring get_sender( ) const;
	};

	///<summary>
	///Интерфейс, содержащий callbacks для получения уведомлений от запущенного метода.
	///</summary>
	///<remarks>
	///<para>Все методы данного интерфейса вызываются асинхронно из другого потока. Не рекомендуется продолжать работу в этом потоке.</para>
	///<para>Лучший вариант - каким-либо образом оповестить основной поток о получении результата и завершиться.</para>
	///</remarks>
	class ISubscriber
	{
	public:
		
		///<summary>
		///В этот метод будет передано любое возникающее уведомление. 
		///</summary>
		virtual void receive_notification( Notification notification ) = 0;

		///<summary>
		///В этот метод будет передан прогресс исполнения в процентах при каждом его изменении. 
		///</summary>
		virtual void progress_changed( double progress ) = 0;

		///<summary>
		///Этот метод вызовется, когда модуль пошлет сигнал о завершении работы.
		///</summary>
		virtual void session_finished( ) = 0;

		virtual ~ISubscriber( ) { }
	};

	typedef HeapPtr<ISubscriber> pISubscriber;

	///<summary>
	///Интерфейс, с помощью которого метод может посылать уведомления пользователю в течение текущей сессии.
	///</summary>
	class INotifier
	{
	public:

		///<summary>
		///Отправляет уведомление всем подписчикам.
		///</summary>
		///<param name="notification">Уведомление, которое будет передано всем подписчикам.</param>
		virtual void notify( Notification notification ) = 0;

		///<summary>
		///Устанавливает прогресс исполнения в процентах.
		///</summary>
		///<param name="progress">Значение текущего прогресса, от 0.0 до 100.0 (с насыщением).</param>
		///<remarks>
		///<para>Если значение прогресса меньше 0, то оно будет установлено в 0.</para>
		///<para>Если значение прогресса больше 100, то оно будет установлено в 100.</para>
		///</remarks>
		virtual void set_progress( double progress ) = 0;

		///<summary>
		///<para>Уведомляет подписчиков о том, что больше уведомлений поступать не будет.</para>
		///<para>Очищает список подписчиков.</para>
		///</summary>
		virtual void notify_of_session_finished( ) = 0;

		/*
			Подписаться на все уведомления.
		*/

		///<summary>
		///Подписаться на все уведомления.
		///</summary>
		///<param name="subscriber">Указатель на объект, который будет получать уведомления.</param>
		virtual void subscribe( const pISubscriber &subscriber ) = 0;
		
		///<summary>
		///Отписаться от уведомлений. Не нужно вызывать после получения события session_finished.
		///</summary>
		///<param name="subscriber">Указатель на объект, который перестанет получать уведомления.</param>
		virtual void unsubscribe( const pISubscriber &subscriber ) = 0;
		
		virtual ~INotifier( ) { }
	};

	typedef HeapPtr<INotifier> pINotifier;
}