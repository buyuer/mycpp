module;
export module mycpp.base:stack;

export namespace mycpp
{

template <class T>
class stack
{
  public:
    stack() = default;

    virtual ~stack() = default;

    virtual void push(const T &) = 0;

    virtual T pop() = 0;

    virtual T &peek() = 0;
};

} // namespace mycpp