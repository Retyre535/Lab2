#include <iostream>
#include <functional>

using namespace std;

class IndexOutOfRange : public exception {
public:
    const char* what() const noexcept override {
        return "Index out of range";
    }
};

template <class T>
class ICollection<T> {
public:
    virtual ~ICollection() = default;
    virtual T Get (int index) = 0;
    virtual int GetSize() = 0;
    virtual void Append(T item) = 0;
    virtual void Prepend(T item) = 0;
    virtual void Insert(T item, int index) = 0;

};

template <class T>
class DynamicArray : public ICollection<T>{
private:
    T *data;
    int size;

public:
    DynamicArray(T* items, int count) : size(count) {
        data = new T[count];
        for (int i = 0; i < count; i++) {
            data[i] = items[i];
        }
    }

    DynamicArray(int size) : size(size) {
        data = new T[size];
    }

    DynamicArray(DynamicArray<T> &dynamicArray) : size(dynamicArray.size()) {
        data = new T[size];
        for (int i = 0; i < size; i++) {
            data[i] = dynamicArray[i];
        }
    };

    ~DynamicArray() {
        delete[] data;
    }

    T Get(int index) override{
        if (index < 0 || index >= size) {
            throw IndexOutOfRange();
        }
        return data[index];
    }

    int GetSize() override{
        return size;
    }

    void Set(int index, T value) {
        if (index < 0 || index >= size) {
            throw IndexOutOfRange();
        }
        data[index] = value;
    }

    void Resize(int newSize) {
        if (newSize <= 0) {
            throw IndexOutOfRange();
        }
        T* newData = new T[newSize];
        //null
        int copySize = (newSize < size) ? newSize : size;
        for (int i = 0; i < copySize; i++) {
            newData[i] = data[i];
        }
        delete[] data;
        data = newData;
        size = newSize;
    }

    T& operator[](int index) {
        if (index < 0 || index >= size) {
            throw IndexOutOfRange();
        }
        return data[index];
    }

    const T& operator[](int index) const {
        if (index < 0 || index >= size) {
            throw IndexOutOfRange();
        }
        return data[index];
    }

    void Append(T item) override{
        Resize(GetSize() + 1);
        data[GetSize() - 1] = item;
    }

    void Prepend(T item) override{
        Resize(GetSize() + 1);
        for (int i = GetSize() - 1; i >= 0; i--) {
            Set(i, item);
        }
    }

    void Insert(T item, int index) override{
        if (index < 0 || index > size) {
            throw IndexOutOfRange();
        }

        if (index == 0) {
            Prepend(item);
        } else if (index == size) {
            Append(item);
        } else {
            Resize(GetSize() + 1);
            for (int i = GetSize() - 1; i >= index; i--) {
                Set(i, item);
            }
            Set(index, item);
        }
    }
};

template <class T>
class LinkedList : public ICollection<T>{
private:
    struct Node {
        T data;
        Node* next;
        Node(T data, Node* next) : data(data), next(next) {}
    };
    Node* head;
    Node* tail;
    int size;

public:
    LinkedList() : head(nullptr), tail(nullptr), size(0) {}

    LinkedList(T* items, int count) : LinkedList() {
        for (int i = 0; i < count; i++) {
            Append(items[i]);
        }
    }

    LinkedList(LinkedList<T> & list) : LinkedList() {
        Node* current = list.head;
        while (current != nullptr) {
            Append(current->data);
            current = current->next;
        }
    }

    ~LinkedList() {
        while (head != nullptr) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
    }

    T GetFirst() {
        if (head == nullptr) {
            throw IndexOutOfRange();
        }
        return head->data;
    }

    T GetLast() {
        if (tail == nullptr) {
            throw IndexOutOfRange();
        }
        return tail->data;
    }

    T Get(int index) override {
        if (index < 0 || index >= size) {
            throw IndexOutOfRange();
        }
        Node* current = head;
        for (int i = 0; i < index; i++) {
            current = current->next;
        }
        return current->data;
    }

    int GetSize() override {
        return size;
    }

    LinkedList<T>* GetSubList(int startIndex, int endIndex) {
        if (startIndex < 0 || endIndex >= size || startIndex > endIndex) {
            throw IndexOutOfRange();
        }

        LinkedList<T>* subList = new LinkedList<T>();
        Node* current = head;

        for (int i = 0; i < startIndex; i++) {
            current = current->next;
        }
        for (int i = startIndex; i < endIndex; i++) {
            subList->Append(current->data);
            current = current->next;
        }
        return subList;
    }

    void Append(T item) override{
        Node* newNode = new Node(item);
        if (head == nullptr) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
        size++;
    }

    void Prepend(T item) override{
        head = new Node(item, head);
        if (tail == nullptr) {
            tail = head;
        }
        size++;
    }

    void Insert(T item, int index) override{
        if (index < 0 || index > size) {
            throw IndexOutOfRange();
        }

        if (index == 0) {
            Prepend(item);
        } else if (index == size) {
            Append(item);
        } else {
            Node* current = head;
            for (int i = 0; i < index-1; i++) {
                current = current->next;
            }
            current->next = new Node(item, current->next);
            size++;
        }
    }

    LinkedList<T>* Concat(LinkedList<T> *list) {
        LinkedList<T>* newList = new LinkedList<T>(*this);
        Node* current = list->head;
        while (current != nullptr) {
            newList->Append(current->data);
            current = current->next;
        }
        return newList;
    }
};

template <class T>
class Sequence : public ICollection<T>{
public:
    virtual ~Sequence() = default;
    virtual T GetFirst() = 0;
    virtual T GetLast() = 0;
    virtual T Get(int index) = 0;
    virtual Sequence<T>* GetSubSequence(int startIndex, int endIndex) = 0;
    virtual int GetSize() = 0;
    virtual void Append(T item) = 0;
    virtual void Prepend(T item) = 0;
    virtual void Insert (T item, int index) = 0;
    virtual Sequence<T>* Concat(Sequence<T>* list) = 0;
    virtual Sequence<T>* Map(function<T(T)> func) = 0;
    virtual T Reduce(function<T(T, T)> func, T startValue) = 0;
    virtual Sequence<T>* Where(function<bool(T)> predicate) = 0;
    virtual Sequence<T>* Zip(Sequence<T>* other, function<T(T, T)> func) = 0;
    virtual Sequence<T>* Slice(int index, int count, Sequence<T>* replacement) = 0;
    virtual Sequence<T>* Split(function<bool(T)> predicate) =0;
    virtual bool TryGet(int index, T& value) = 0;
    virtual bool TryFind(function<bool(T)> predicate, T& value) = 0;
    virtual T& operator[](int index) = 0;
    virtual const T& operator[](int index) const = 0;
};

template <class T>
class MutableArraySequence : public Sequence<T>, public ICollection<T> {
protected:
    DynamicArray<T>* array;
    MutableArraySequence<T>* CreateMutableArraySequence(){
        return new MutableArraySequence<T>();
    }
public:
    MutableArraySequence(T* items, int count) {
        array = new DynamicArray<T>(items, count);
    }

    MutableArraySequence() {
        array = new DynamicArray<T>(0);
    }

    MutableArraySequence(MutableArraySequence<T>* other) {
        array = new DynamicArray<T>(*other.array);
    }

    ~MutableArraySequence(){
        delete array;
    }

    Sequence<T>* GetSubSequence(int startIndex, int endIndex) override {
        Sequence<T>* subSequence = new MutableArraySequence<T>();
        for (int i = startIndex; i < endIndex; i++) {
            subSequence->Append(Get(i));
        }
        return subSequence;
    }

    T GetFirst() override{
        if (array->GetSize() == 0) {
            throw IndexOutOfRange();
        }
        return array->Get(0);
    }

    T GetLast() override{
        if (array->GetSize() == 0) {
            throw IndexOutOfRange();
        }
        return array->Get(array->GetSize() - 1);
    }

    T Get(int index) override{
        return array->Get(index);
    }

    int GetSize() override{
        return array->GetSize();
    }

    T& operator[](int index) override {
        return (*array)[index];
    }

    const T& operator[](int index) const override {
        return (*array)[index];
    }

    bool TryGet(int index, T& value) override{
        if (index < 0 || index >= array->GetSize()) {
            throw IndexOutOfRange();
        }
        value = array->Get(index);
        return true;
    }

    bool TryFind(function<bool(T)> predicate, T& value) override{
        for (int i = 0; i < array->GetSize(); i++) {
            if (predicate((*array)[i])) {
                value = array->Get(i);
                return true;
            }
        }
        return false;
    }

    Sequence<T>* Map(function<bool(T)> func) override{
        MutableArraySequence<T>* newSequence = CreateMutableArraySequence();
        for (int i = 0; i < array->GetSize(); ++i) {
            newSequence->Append(func(array->Get(i)));
        }
        return newSequence;
    }

    T Reduce(function<T(T, T)> func, T startValue) override{
        T result = startValue;
        for (int i = 0; i < array->GetSize(); ++i) {
            result = func(result, array->Get(i));
        }
        return result;
    }

    Sequence<T>* Where(function<bool(T)> predicate) override{
        MutableArraySequence<T>* newSequence = CreateMutableArraySequence();
        for (int i = 0; i < array->GetSize(); ++i) {
            newSequence->Append(array->Get(i));
        }
        return newSequence;
    }

    Sequence<T>* Zip (Sequence<T>* other, function<T(T, T)> func) override {
        MutableArraySequence<T>* newSequence = CreateMutableArraySequence();
        int minLength = min(array->GetSize(), other->GetSize());
        for (int i = 0; i < minLength; ++i) {
            newSequence->Append(func(array->Get(i), other->Get(i)));
        }
        return newSequence;
    }

    Sequence<T>* Slice(int index, int count, Sequence<T>* replacement) override {
        MutableArraySequence<T>* newSequence = CreateMutableArraySequence();
        if (index < 0) {
            index = array->GetSize() + index;
            if (index < 0) {
                throw IndexOutOfRange();
            }
        }
        if (index >= array->GetSize() || index + count > array->GetSize()) {
            throw IndexOutOfRange();
        }
        for (int i = 0; i < index; ++i) {
            newSequence->Append(array->Get(i));
        }
        if (replacement != nullptr) {
            for (int i = 0; i < replacement->GetSize(); ++i) {
                newSequence->Append(replacement->Get(i));
            }
        }
        for (int i = index + count; i < array->GetSize(); ++i) {
            newSequence->Append(array->Get(i));
        }
        return newSequence;
    }

    Sequence<T>* Split(function<bool(T)> predicate) override {
        MutableArraySequence<T>* newSequence = CreateMutableArraySequence();
        MutableArraySequence<T>* currentChunk = CreateMutableArraySequence();
        for (int i = 0; i < array->GetSize(); ++i) {
            T item = array->Get(i);
            if (predicate(item)) {
                if (currentChunk->GetSize() > 0) {
                    newSequence->Concat(currentChunk);
                    currentChunk = CreateMutableArraySequence();
                }
            } else {
                    currentChunk->Append(item);
                }
            }
            if (currentChunk->GetSize() > 0) {
                newSequence->Concat(currentChunk);
            }
            return newSequence;
    }

    void Append(T item) override{
        this->array->Append(item);
    }

    void Prepend(T item) override{
        this->array->Prepend(item);
    }

    void Insert(T item, int index) override{
        this->array->Insert(item, index);
    }

    Sequence<T>* Concat(Sequence<T>* list) override{
        MutableArraySequence<T>* newSequence = new Sequence<T>(*this);
        for (int i = 0; i < list->GetSize(); ++i) {
            newSequence->Append(list->Get(i));
        }
        return newSequence;
    }
};

template <class T>
class ImmutableArraySequence : public Sequence<T>, public ICollection<T>{
protected:
    DynamicArray<T>* array;
    ImmutableArraySequence<T>* CreateImmutableArraySequence(){
        return new ImmutableArraySequence<T>();
    }
public:
    ImmutableArraySequence(T* items, int count) {
        array = new DynamicArray<T>(items, count);
    }

    ImmutableArraySequence() {
        array = new DynamicArray<T>(0);
    }

    ImmutableArraySequence(MutableArraySequence<T>* other) {
        array = new DynamicArray<T>(*other.array);
    }

    ~ImmutableArraySequence(){
        delete array;
    }

    Sequence<T>* GetSubSequence(int startIndex, int endIndex) override {
        Sequence<T>* subSequence = new ImmutableArraySequence<T>();
        for (int i = startIndex; i < endIndex; i++) {
            subSequence->Append(Get(i));
        }
        return subSequence;
    }

    T GetFirst() override{
        if (array->GetSize() == 0) {
            throw IndexOutOfRange();
        }
        return array->Get(0);
    }

    T GetLast() override{
        if (array->GetSize() == 0) {
            throw IndexOutOfRange();
        }
        return array->Get(array->GetSize() - 1);
    }

    T Get(int index) override{
        return array->Get(index);
    }

    int GetSize() override{
        return array->GetSize();
    }

    T& operator[](int index) override {
        return (*array)[index];
    }

    const T& operator[](int index) const override {
        return (*array)[index];
    }

    bool TryGet(int index, T& value) override{
        if (index < 0 || index >= array->GetSize()) {
            throw IndexOutOfRange();
        }
        value = array->Get(index);
        return true;
    }

    bool TryFind(function<bool(T)> predicate, T& value) override{
        for (int i = 0; i < array->GetSize(); i++) {
            if (predicate((*array)[i])) {
                value = array->Get(i);
                return true;
            }
        }
        return false;
    }

    Sequence<T>* Map(function<bool(T)> func) override{
        MutableArraySequence<T>* newSequence = CreateImmutableArraySequence();
        for (int i = 0; i < array->GetSize(); ++i) {
            newSequence->Append(func(array->Get(i)));
        }
        return newSequence;
    }

    T Reduce(function<T(T, T)> func, T startValue) override{
        T result = startValue;
        for (int i = 0; i < array->GetSize(); ++i) {
            result = func(result, array->Get(i));
        }
        return result;
    }

    Sequence<T>* Where(function<bool(T)> predicate) override{
        MutableArraySequence<T>* newSequence = CreateImmutableArraySequence();
        for (int i = 0; i < array->GetSize(); ++i) {
            newSequence->Append(array->Get(i));
        }
        return newSequence;
    }

    Sequence<T>* Zip (Sequence<T>* other, function<T(T, T)> func) override {
        MutableArraySequence<T>* newSequence = CreateImmutableArraySequence();
        int minLength = min(array->GetSize(), other->GetSize());
        for (int i = 0; i < minLength; ++i) {
            newSequence->Append(func(array->Get(i), other->Get(i)));
        }
        return newSequence;
    }

    Sequence<T>* Slice(int index, int count, Sequence<T>* replacement) override {
        MutableArraySequence<T>* newSequence = CreateImmutableArraySequence();
        if (index < 0) {
            index = array->GetSize() + index;
            if (index < 0) {
                throw IndexOutOfRange();
            }
        }
        if (index >= array->GetSize() || index + count > array->GetSize()) {
            throw IndexOutOfRange();
        }
        for (int i = 0; i < index; ++i) {
            newSequence->Append(array->Get(i));
        }
        if (replacement != nullptr) {
            for (int i = 0; i < replacement->GetSize(); ++i) {
                newSequence->Append(replacement->Get(i));
            }
        }
        for (int i = index + count; i < array->GetSize(); ++i) {
            newSequence->Append(array->Get(i));
        }
        return newSequence;
    }

    Sequence<T>* Split(function<bool(T)> predicate) override {
        MutableArraySequence<T>* newSequence = CreateImmutableArraySequence();
        MutableArraySequence<T>* currentChunk = CreateImmutableArraySequence();
        for (int i = 0; i < array->GetSize(); ++i) {
            T item = array->Get(i);
            if (predicate(item)) {
                if (currentChunk->GetSize() > 0) {
                    newSequence->Concat(currentChunk);
                    currentChunk = CreateImmutableArraySequence();
                }
            } else {
                    currentChunk->Append(item);
                }
            }
            if (currentChunk->GetSize() > 0) {
                newSequence->Concat(currentChunk);
            }
            return newSequence;
    }

    void Append(T item) override{
        this->array->Append(item);
    }

    void Prepend(T item) override{
        this->array->Prepend(item);
    }

    void Insert(T item, int index) override{
        this->array->Insert(item, index);
    }

    Sequence<T>* Concat(Sequence<T>* list) override{
        ImmutableArraySequence<T>* newSequence = new Sequence<T>(*this);
        for (int i = 0; i < list->GetSize(); ++i) {
            newSequence->Append(list->Get(i));
        }
        return newSequence;
    }
};

template <class T>
class MutableListSequence : public Sequence<T>, public ICollection<T> {
protected:
    LinkedList<T>* list;
    MutableListSequence<T>* CreateMutableListSequence(){
        return new MutableListSequence<T>();
    }
public:
    MutableListSequence(T* items, int count) {
        list = new LinkedList<T>(items, count);
    }

    MutableListSequence() {
        list = new LinkedList<T>();
    }

    MutableListSequence(MutableListSequence<T>* other) {
        list = new LinkedList<T>(*other);
    }

    ~MutableListSequence() {
        delete this->list;
    }

    Sequence<T>* GetSubSequence(int startIndex, int endIndex) override {
        Sequence<T>* subSequence = new MutableListSequence<T>();
        for (int i = startIndex; i < endIndex; i++) {
            subSequence->Append(Get(i));
        }
        return subSequence;
    }

    T GetFirst() override{
        if (list->GetSize() == 0) {
            throw IndexOutOfRange();
        }
        return list->Get(0);
    }

    T GetLast() override{
        if (list->GetSize() == 0) {
            throw IndexOutOfRange();
        }
        return list->GetLast();
    }

    T Get(int index) override{
        return list->Get(index);
    }

    int GetSize() override{
        return list->GetSize();
    }

    T& operator[](int index) override {
        return (*list)[index];
    }

    const T& operator[](int index) const override {
        return (*list)[index];
    }

    bool TryGet(int index, T& value) override{
        if (index < 0 || index >= list->GetSize()) {
            throw IndexOutOfRange();
        }
        value = list->Get(index);
        return true;
    }

    bool TryFind(function<bool(T)> predicate, T& value) override{
        for (int i = 0; i < list->GetSize(); i++) {
            if (predicate((*list)[i])) {
                value = list->Get(i);
                return true;
            }
        }
        return false;
    }

    Sequence<T>* Map(function<bool(T)> func) override{
        MutableListSequence<T>* newSequence = CreateMutableListSequence();
        for (int i = 0; i < list->GetSize(); ++i) {
            newSequence->Append(func(list->Get(i)));
        }
        return newSequence;
    }

    T Reduce(function<T(T, T)> func, T startValue) override{
        T result = startValue;
        for (int i = 0; i < list->GetSize(); ++i) {
            result = func(result, list->Get(i));
        }
        return result;
    }

    Sequence<T>* Where(function<bool(T)> predicate) override{
        MutableListSequence<T>* newSequence = CreateMutableListSequence();
        for (int i = 0; i < list->GetSize(); ++i) {
            newSequence->Append(list->Get(i));
        }
        return newSequence;
    }

    Sequence<T>* Zip (Sequence<T>* other, function<T(T, T)> func) override {
        MutableListSequence<T>* newSequence = CreateMutableListSequence();
        int minLength = min(list->GetSize(), other->GetSize());
        for (int i = 0; i < minLength; ++i) {
            newSequence->Append(func(list->Get(i), other->Get(i)));
        }
        return newSequence;
    }

    Sequence<T>* Slice(int index, int count, Sequence<T>* replacement) override {
        MutableListSequence<T>* newSequence = CreateMutableListSequence();
        if (index < 0) {
            index = list->GetSize() + index;
            if (index < 0) {
                throw IndexOutOfRange();
            }
        }
        if (index >= list->GetSize() || index + count > list->GetSize()) {
            throw IndexOutOfRange();
        }
        for (int i = 0; i < index; ++i) {
            newSequence->Append(list->Get(i));
        }
        if (replacement != nullptr) {
            for (int i = 0; i < replacement->GetSize(); ++i) {
                newSequence->Append(replacement->Get(i));
            }
        }
        for (int i = index + count; i < list->GetSize(); ++i) {
            newSequence->Append(list->Get(i));
        }
        return newSequence;
    }

    Sequence<T>* Split(function<bool(T)> predicate) override {
        MutableListSequence<T>* newSequence = CreateMutableListSequence();
        MutableListSequence<T>* currentChunk = CreateMutableListSequence();
        for (int i = 0; i < list->GetSize(); ++i) {
            T item = list->Get(i);
            if (predicate(item)) {
                if (currentChunk->GetSize() > 0) {
                    newSequence->Concat(currentChunk);
                    currentChunk = CreateMutableListSequence();
                }
            } else {
                    currentChunk->Append(item);
                }
            }
            if (currentChunk->GetSize() > 0) {
                newSequence->Concat(currentChunk);
            }
            return newSequence;
    }

    void Append(T item) override{
        this->list->Append(item);
    }

    void Prepend(T item) override{
        this->list->Prepend(item);
    }

    void Insert(T item, int index) override{
        this->list->InsertAt(item, index);
    }

    Sequence<T>* Concat(Sequence<T>* list) override{
        Sequence<T> newSequence = new MutableListSequence<T>(*this);
        for (int i = 0; i < list->GetSize(); ++i) {
            newSequence->Append(list->Get(i));
        }
        return newSequence;
    }
};

template <class T>
class ImmutableListSequence : public Sequence<T>, public ICollection<T>{
protected:
    LinkedList<T>* list;
    ImmutableListSequence<T>* CreateImmutableListSequence(){
        return new ImmutableListSequence<T>();
    }
public:
    ImmutableListSequence(T* items, int count) {
        list = new LinkedList<T>(items, count);
    }

    ImmutableListSequence() {
        list = new LinkedList<T>();
    }

    ImmutableListSequence(MutableListSequence<T>* other) {
        list = new LinkedList<T>(*other);
    }

    ~ImmutableListSequence() {
        delete this->list;
    }

    Sequence<T>* GetSubSequence(int startIndex, int endIndex) override {
        Sequence<T>* subSequence = new ImmutableArraySequence<T>();
        for (int i = startIndex; i < endIndex; i++) {
            subSequence->Append(Get(i));
        }
        return subSequence;
    }

    T GetFirst() override{
        if (list->GetSize() == 0) {
            throw IndexOutOfRange();
        }
        return list->Get(0);
    }

    T GetLast() override{
        if (list->GetSize() == 0) {
            throw IndexOutOfRange();
        }
        return list->GetLast();
    }

    T Get(int index) override{
        return list->Get(index);
    }

    int GetSize() override{
        return list->GetSize();
    }

    T& operator[](int index) override {
        return (*list)[index];
    }

    const T& operator[](int index) const override {
        return (*list)[index];
    }

    bool TryGet(int index, T& value) override{
        if (index < 0 || index >= list->GetSize()) {
            throw IndexOutOfRange();
        }
        value = list->Get(index);
        return true;
    }

    bool TryFind(function<bool(T)> predicate, T& value) override{
        for (int i = 0; i < list->GetSize(); i++) {
            if (predicate((*list)[i])) {
                value = list->Get(i);
                return true;
            }
        }
        return false;
    }

    Sequence<T>* Map(function<bool(T)> func) override{
        MutableListSequence<T>* newSequence = CreateImmutableListSequence();
        for (int i = 0; i < list->GetSize(); ++i) {
            newSequence->Append(func(list->Get(i)));
        }
        return newSequence;
    }

    T Reduce(function<T(T, T)> func, T startValue) override{
        T result = startValue;
        for (int i = 0; i < list->GetSize(); ++i) {
            result = func(result, list->Get(i));
        }
        return result;
    }

    Sequence<T>* Where(function<bool(T)> predicate) override{
        MutableListSequence<T>* newSequence = CreateImmutableListSequence();
        for (int i = 0; i < list->GetSize(); ++i) {
            newSequence->Append(list->Get(i));
        }
        return newSequence;
    }

    Sequence<T>* Zip (Sequence<T>* other, function<T(T, T)> func) override {
        MutableListSequence<T>* newSequence = CreateImmutableListSequence();
        int minLength = min(list->GetSize(), other->GetSize());
        for (int i = 0; i < minLength; ++i) {
            newSequence->Append(func(list->Get(i), other->Get(i)));
        }
        return newSequence;
    }

    Sequence<T>* Slice(int index, int count, Sequence<T>* replacement) override {
        MutableListSequence<T>* newSequence = CreateImmutableListSequence();
        if (index < 0) {
            index = list->GetSize() + index;
            if (index < 0) {
                throw IndexOutOfRange();
            }
        }
        if (index >= list->GetSize() || index + count > list->GetSize()) {
            throw IndexOutOfRange();
        }
        for (int i = 0; i < index; ++i) {
            newSequence->Append(list->Get(i));
        }
        if (replacement != nullptr) {
            for (int i = 0; i < replacement->GetSize(); ++i) {
                newSequence->Append(replacement->Get(i));
            }
        }
        for (int i = index + count; i < list->GetSize(); ++i) {
            newSequence->Append(list->Get(i));
        }
        return newSequence;
    }

    Sequence<T>* Split(function<bool(T)> predicate) override {
        MutableListSequence<T>* newSequence = CreateImmutableListSequence();
        MutableListSequence<T>* currentChunk = CreateImmutableListSequence();
        for (int i = 0; i < list->GetSize(); ++i) {
            T item = list->Get(i);
            if (predicate(item)) {
                if (currentChunk->GetSize() > 0) {
                    newSequence->Concat(currentChunk);
                    currentChunk = CreateImmutableListSequence();
                }
            } else {
                    currentChunk->Append(item);
                }
            }
            if (currentChunk->GetSize() > 0) {
                newSequence->Concat(currentChunk);
            }
            return newSequence;
    }

    void Append(T item) override{
        this->list->Append(item);
    }

    void Prepend(T item) override{
        this->list->Prepend(item);
    }

    void Insert(T item, int index) override{
        this->list->InsertAt(item, index);
    }

    Sequence<T>* Concat(Sequence<T>* list) override{
        Sequence<T> newSequence = new ImmutableListSequence<T>(*this);
        for (int i = 0; i < list->GetSize(); ++i) {
            newSequence->Append(list->Get(i));
        }
        return newSequence;
    }
};

template <class T>
class AdaptiveSequence : public Sequence<T> {
private:
    enum class StorageType { Array, List };
    StorageType currentType;
    ICollection<T>* storage;
    size_t operationThreshold = 10;
    size_t randomAccessCount = 0;
    size_t insertionCount = 0;

    void InitializeStorage(StorageType type) {
        if (storage) delete storage;

        if (type == StorageType::Array) {
            storage = new DynamicArray<T>();
        } else {
            storage = new LinkedList<T>();
        }
        currentType = type;
    }

    void CheckAndSwitch() {
        if (randomAccessCount > operationThreshold && currentType == StorageType::List) {
            SwitchToArray();
        } else if (insertionCount > operationThreshold && currentType == StorageType::Array) {
            SwitchToList();
        }
    }

    void SwitchToArray() {
        if (currentType == StorageType::Array) return;
        DynamicArray<T>* newArray = new DynamicArray<T>();
        for (int i = 0; i < storage->GetSize(); ++i) {
            newArray->Append(storage->Get(i));
        }
        delete storage;
        storage = newArray;
        currentType = StorageType::Array;
        randomAccessCount = 0;
        insertionCount = 0;
    }

    void SwitchToList() {
        if (currentType == StorageType::List) return;
        LinkedList<T>* newList = new LinkedList<T>();
        for (int i = 0; i < storage->GetSize(); ++i) {
            newList->Append(storage->Get(i));
        }
        delete storage;
        storage = newList;
        currentType = StorageType::List;
        randomAccessCount = 0;
        insertionCount = 0;
    }

public:
    AdaptiveSequence() {
        InitializeStorage(StorageType::Array);
    }

    AdaptiveSequence(T* items, int count) {
        InitializeStorage(count > 100 ? StorageType::Array : StorageType::List);
        for (int i = 0; i < count; ++i) {
            storage->Append(items[i]);
        }
    }

    AdaptiveSequence(AdaptiveSequence<T>& other) {
        InitializeStorage(other.currentType);
        for (int i = 0; i < other.storage->GetSize(); ++i) {
            storage->Append(other.storage->Get(i));
        }
    }

    ~AdaptiveSequence() {
        delete storage;
    }

    T GetFirst() override {
        if (storage->GetSize() == 0) throw IndexOutOfRange();
        return storage->Get(0);
    }

    T GetLast() override {
        int size = storage->GetSize();
        if (size == 0) throw IndexOutOfRange();
        return storage->Get(size - 1);
    }

    T Get(int index) override {
        randomAccessCount++;
        CheckAndSwitch();
        return storage->Get(index);
    }

    int GetSize() override {
        return storage->GetSize();
    }

    T& operator[](int index) override {
        randomAccessCount++;
        CheckAndSwitch();
        if (currentType == StorageType::Array) {
            return dynamic_cast<DynamicArray<T>*>(storage)->operator[](index);
        }
        throw std::runtime_error("Operator[] not supported for list storage");
    }

    const T& operator[](int index) const override {
        if (currentType == StorageType::Array) {
            return dynamic_cast<const DynamicArray<T>*>(storage)->operator[](index);
        }
        throw std::runtime_error("Operator[] not supported for list storage");
    }

    void Append(T item) override {
        insertionCount++;
        CheckAndSwitch();
        storage->Append(item);
    }

    void Prepend(T item) override {
        insertionCount++;
        CheckAndSwitch();
        storage->Prepend(item);
    }

    void Insert(T item, int index) override {
        insertionCount++;
        CheckAndSwitch();
        storage->Insert(item, index);
    }

    Sequence<T>* GetSubSequence(int startIndex, int endIndex) override {
        AdaptiveSequence<T>* subSequence = new AdaptiveSequence<T>();
        for (int i = startIndex; i < endIndex; i++) {
            subSequence->Append(Get(i));
        }
        return subSequence;
    }

    Sequence<T>* Concat(Sequence<T>* other) override {
        AdaptiveSequence<T>* newSequence = new AdaptiveSequence<T>(*this);
        for (int i = 0; i < other->GetSize(); i++) {
            newSequence->Append(other->Get(i));
        }
        return newSequence;
    }

    Sequence<T>* Map(function<T(T)> func) override {
        AdaptiveSequence<T>* newSequence = new AdaptiveSequence<T>();
        int size = GetSize();
        for (int i = 0; i < size; ++i) {
            newSequence->Append(func(Get(i)));
        }
        return newSequence;
    }

    T Reduce(function<T(T, T)> func, T startValue) override {
        T result = startValue;
        int size = GetSize();
        for (int i = 0; i < size; ++i) {
            result = func(result, Get(i));
        }
        return result;
    }

    Sequence<T>* Where(function<bool(T)> predicate) override {
        AdaptiveSequence<T>* newSequence = new AdaptiveSequence<T>();
        int size = GetSize();
        for (int i = 0; i < size; ++i) {
            T item = Get(i);
            if (predicate(item)) {
                newSequence->Append(item);
            }
        }
        return newSequence;
    }

    Sequence<T>* Zip(Sequence<T>* other, function<T(T, T)> func) override {
        AdaptiveSequence<T>* newSequence = new AdaptiveSequence<T>();
        int minSize = min(GetSize(), other->GetSize());
        for (int i = 0; i < minSize; ++i) {
            newSequence->Append(func(Get(i), other->Get(i)));
        }
        return newSequence;
    }

    Sequence<T>* Slice(int index, int count, Sequence<T>* replacement) override {
        AdaptiveSequence<T>* newSequence = new AdaptiveSequence<T>();
        for (int i = 0; i < index; ++i) {
            newSequence->Append(Get(i));
        }
        if (replacement != nullptr) {
            for (int i = 0; i < replacement->GetSize(); ++i) {
                newSequence->Append(replacement->Get(i));
            }
        }
        for (int i = index + count; i < GetSize(); ++i) {
            newSequence->Append(Get(i));
        }
        return newSequence;
    }

    Sequence<T>* Split(function<bool(T)> predicate) override {
        AdaptiveSequence<T>* result = new AdaptiveSequence<T>();
        AdaptiveSequence<T>* current = new AdaptiveSequence<T>();
        int size = GetSize();
        for (int i = 0; i < size; ++i) {
            T item = Get(i);
            if (predicate(item)) {
                if (current->GetSize() > 0) {
                    result->Append(current->Get(0));
                    delete current;
                    current = new AdaptiveSequence<T>();
                }
            } else {
                current->Append(item);
            }
        }

        if (current->GetSize() > 0) {
            result->Append(current->Get(0));
        } else {
            delete current;
        }

        return result;
    }

    bool TryGet(int index, T& value) override {
        if (index < 0 || index >= GetSize()) {
            return false;
        }
        value = Get(index);
        return true;
    }

    bool TryFind(function<bool(T)> predicate, T& value) override {
        int size = GetSize();
        for (int i = 0; i < size; ++i) {
            T item = Get(i);
            if (predicate(item)) {
                value = item;
                return true;
            }
        }
        return false;
    }

    void OptimizeForRandomAccess() {
        SwitchToArray();
    }

    void OptimizeForInsertions() {
        SwitchToList();
    }

    bool IsArray() const {
        return currentType == StorageType::Array;
    }

    bool IsList() const {
        return currentType == StorageType::List;
    }

    void SetOperationThreshold(size_t threshold) {
        operationThreshold = threshold;
    }
};

template <class T>
class SegmentedList : public ICollection<T> {
private:
    static const size_t SEGMENT_SIZE = 32;

    struct Segment {
        T data[SEGMENT_SIZE];
        size_t size = 0;
        Segment* next = nullptr;

        ~Segment() {
            delete next;
        }
    };

    Segment* head = nullptr;
    size_t totalSize = 0;

    pair<Segment*, size_t> GetSegment(size_t index) {
        if (index >= totalSize) {
            throw IndexOutOfRange();
        }
        Segment* current = head;
        while (index >= SEGMENT_SIZE && current->next) {
            index -= SEGMENT_SIZE;
            current = current->next;
        }
        return {current, index};
    }

public:
    ~SegmentedList() {
        delete head;
    }

    T Get(int index) override {
        pair<Segment*, size_t> segmentInfo = GetSegment(index);
        return segmentInfo.first->data[segmentInfo.second];
    }

    int GetSize() override {
        return totalSize;
    }

    void Append(T item) override {
        if (!head) {
            head = new Segment();
        }

        Segment* current = head;
        while (current->next) {
            current = current->next;
        }

        if (current->size == SEGMENT_SIZE) {
            current->next = new Segment();
            current = current->next;
        }

        current->data[current->size++] = item;
        totalSize++;
    }

    void Prepend(T item) override {
        if (!head) {
            head = new Segment();
        }
        if (head->size == SEGMENT_SIZE) {
            Segment* newSegment = new Segment();
            newSegment->next = head;
            head = newSegment;
        }
        for (size_t i = head->size; i > 0; --i) {
            head->data[i] = head->data[i - 1];
        }
        head->data[0] = item;
        head->size++;
        totalSize++;
    }

    void Insert(T item, int index) override {
        if (index == 0) {
            Prepend(item);
            return;
        }
        if (index == totalSize) {
            Append(item);
            return;
        }
        pair<Segment*, size_t> segmentInfo = GetSegment(index);
        Segment* segment = segmentInfo.first;
        size_t offset = segmentInfo.second;
        if (segment->size == SEGMENT_SIZE) {
            Segment* newSegment = new Segment();
            size_t moveCount = SEGMENT_SIZE / 2;
            size_t startIndex = SEGMENT_SIZE - moveCount;
            for (size_t i = 0; i < moveCount; ++i) {
                newSegment->data[i] = segment->data[startIndex + i];
            }
            newSegment->size = moveCount;
            segment->size -= moveCount;
            newSegment->next = segment->next;
            segment->next = newSegment;
            if (offset >= segment->size) {
                offset -= segment->size;
                segment = newSegment;
            }
        }
        for (size_t i = segment->size; i > offset; --i) {
            segment->data[i] = segment->data[i - 1];
        }
        segment->data[offset] = item;
        segment->size++;
        totalSize++;
    }
};

template <class T>
class Option {
private:
    T value;
    bool hasValue;
public:
    Option() : hasValue(false) {}
    Option(T val) : value(val), hasValue(true) {}
    static Option<T> None() {
        return Option<T>();
    }

    bool IsSome() {
        return hasValue;
    }

    bool IsNone() {
        return !hasValue;
    }

    T GetValue() {
        if (!hasValue) {
            throw runtime_error("Option is None");
        }
        return value;
    }
};

int main() {
    return 0;
}