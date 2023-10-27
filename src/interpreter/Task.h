#ifndef RINHA_INTERPRETER_TASK_H
#define RINHA_INTERPRETER_TASK_H

#include "./Values.h"
#include <coroutine>
#include <exception>
#include <optional>
#include <utility>
#include <variant>


namespace rinha::interpreter
{
	class Task
	{
	public:
		class Promise
		{
		public:
			class FinalAwaiter
			{
			public:
				bool await_ready() noexcept
				{
					return false;
				}

				std::coroutine_handle<> await_suspend(std::coroutine_handle<Promise> coroHandle) noexcept
				{
					return coroHandle.promise().continuation;
				}

				void await_resume() noexcept { }
			};

		public:
			Task get_return_object() noexcept
			{
				return Task{std::coroutine_handle<Promise>::from_promise(*this)};
			}

			std::suspend_always initial_suspend() noexcept
			{
				return {};
			}

			void return_value(Value value) noexcept
			{
				result = std::move(value);
			}

			void unhandled_exception() noexcept
			{
				result = std::current_exception();
			}

			FinalAwaiter final_suspend() noexcept
			{
				return {};
			}

			std::coroutine_handle<> continuation;
			std::variant<std::monostate, Value, std::exception_ptr> result;
		};

		using promise_type = Promise;

		class Awaiter
		{
		public:
			bool await_ready() noexcept
			{
				return false;
			}

			std::coroutine_handle<> await_suspend(std::coroutine_handle<> continuation) noexcept
			{
				// Store the continuation in the task's promise so that the final_suspend()
				// knows to resume this coroutine when the task completes.
				coroHandle.promise().continuation = continuation;

				// Then we resume the task's coroutine, which is currently suspended
				// at the initial-suspend-point (ie. at the open curly brace).
				return coroHandle;
			}

			Value await_resume()
			{
				const auto& promise = coroHandle.promise();

				if (auto value = std::get_if<Value>(&promise.result))
					return *value;

				std::rethrow_exception(std::get<std::exception_ptr>(promise.result));
			}

		private:
			friend Task;

			explicit Awaiter(std::coroutine_handle<Promise> coroHandle) noexcept
				: coroHandle(coroHandle)
			{
			}

		private:
			std::coroutine_handle<Promise> coroHandle;
		};

	public:
		Task(Task&& t) noexcept
			: coroHandle(std::exchange(t.coroHandle, {}))
		{
		}

		~Task()
		{
			if (coroHandle)
				coroHandle.destroy();
		}

		Awaiter operator co_await() && noexcept
		{
			return Awaiter{coroHandle};
		}

	private:
		explicit Task(std::coroutine_handle<Promise> coroHandle) noexcept
			: coroHandle(coroHandle)
		{
		}

	private:
		std::coroutine_handle<Promise> coroHandle;
	};

	class SyncWaitTask
	{
	public:
		class Promise
		{
			friend class ManualExecutor;

		public:
			SyncWaitTask get_return_object() noexcept
			{
				return SyncWaitTask{std::coroutine_handle<Promise>::from_promise(*this)};
			}

			std::suspend_never initial_suspend() noexcept
			{
				return {};
			}

			std::suspend_always final_suspend() noexcept
			{
				return {};
			}

			void return_value(Value value) noexcept
			{
				result = std::move(value);
			}

			void unhandled_exception() noexcept
			{
				result = std::current_exception();
			}

		private:
			std::variant<std::monostate, Value, std::exception_ptr> result;
		};

		using promise_type = Promise;

	public:
		explicit SyncWaitTask(std::coroutine_handle<Promise> coroHandle) noexcept
			: coroHandle(coroHandle)
		{
		}

		SyncWaitTask(SyncWaitTask&& task) noexcept
			: coroHandle(task.coroHandle)
		{
			task.coroHandle = {};
		}

		~SyncWaitTask()
		{
			if (coroHandle)
				coroHandle.destroy();
		}

	public:
		static SyncWaitTask start(Task&& task)
		{
			co_return co_await std::move(task);
		}

	public:
		bool done()
		{
			return coroHandle.done();
		}

	public:
		std::coroutine_handle<Promise> coroHandle;
	};

	class ManualExecutor
	{
	public:
		class ScheduleOp
		{
		public:
			ScheduleOp(ManualExecutor& executor)
				: executor(executor)
			{
			}

		public:
			bool await_ready() noexcept
			{
				return false;
			}

			void await_suspend(std::coroutine_handle<> aContinuation) noexcept
			{
				continuation = aContinuation;
				next = executor.head;
				executor.head = this;
			}

			void await_resume() noexcept { }

		public:
			ManualExecutor& executor;
			ScheduleOp* next = nullptr;
			std::coroutine_handle<> continuation;
		};

	public:
		ScheduleOp schedule() noexcept
		{
			return ScheduleOp{*this};
		}

		void drain()
		{
			while (head != nullptr)
			{
				auto* item = head;
				head = item->next;
				item->continuation.resume();
			}
		}

		Value syncWait(Task&& task)
		{
			auto syncWaitTask = SyncWaitTask::start(std::move(task));

			while (!syncWaitTask.done())
				drain();

			const auto& promise = syncWaitTask.coroHandle.promise();

			if (auto value = std::get_if<Value>(&promise.result))
				return *value;

			std::rethrow_exception(std::get<std::exception_ptr>(promise.result));
		}

	private:
		ScheduleOp* head = nullptr;
	};
}  // namespace rinha::interpreter

#endif  // RINHA_INTERPRETER_TASK_H
