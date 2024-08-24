template <typename T>
void VectorDebugger<T>::print(const std::vector<T> &v)
{
    for (typename std::vector<T>::const_iterator it = v.begin(); it != v.end(); it++)
        std::cout << (*it) << std::endl;
}