#include "future-promise.cpp"


void TestPromise()
{
    Promise<int> promise;
    Future<int> future = promise.GetFuture();
    promise.SetValue(10);
    assert(future.Get() == 10);
}

void TestPromiseWithException()
{
    Promise<int> promise;
    Future<int> future = promise.GetFuture();
    std::runtime_error error("test error");
    promise.SetException(error);
    try
    {
        future.Get();
        assert(false);
    }
    catch (std::runtime_error& ex)
    {
        assert(std::string(ex.what()) == "test error");
    }
}

void TestFutureTryGet()
{
    Promise<int> promise;
    Future<int> future = promise.GetFuture();
    int value;
    assert(!future.TryGet(value));
    promise.SetValue(10);
    assert(future.TryGet(value));
    assert(value == 10);
}

void RunTests()
{
    TestPromise();
    // TestPromiseWithException();
    // TestFutureTryGet();
}

int main()
{
    RunTests();
    return 0;
}
