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
class DynamicArray {
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

    T Get(int index) {
        if (index < 0 || index >= size) {
            throw IndexOutOfRange();
        }
        return data[index];
    }

    int GetSize() {
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
};

template <class T>
class LinkedList {
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

    T Get(int index) {
        if (index < 0 || index >= size) {
            throw IndexOutOfRange();
        }
        Node* current = head;
        for (int i = 0; i < index; i++) {
            current = current->next;
        }
        return current->data;
    }

    int GetLength() {
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

    void Append(T item) {
        Node* newNode = new Node(item);
        if (head == nullptr) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
        size++;
    }

    void Prepend(T item) {
        head = new Node(item, head);
        if (tail == nullptr) {
            tail = head;
        }
        size++;
    }

    void Insert(int index, T item) {
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
class Sequence {
public:
    virtual ~Sequence() = default;
    virtual T GetFirst() = 0;
    virtual T GetLast() = 0;
    virtual T Get(int index) = 0;
    virtual Sequence<T>* GetSubSequence(int startIndex, int endIndex) = 0;
    virtual int GetLength() = 0;
    virtual Sequence<T>* Append(T item) = 0;
    virtual Sequence<T>* Prepend(T item) = 0;
    virtual Sequence<T>* InsertAt (T item, int index) = 0;
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
class MutableArraySequence : public Sequence<T> {
protected:
    DynamicArray<T>* array;
    virtual MutableArraySequence<T>* CreateMutableArraySequence() = 0;
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
        Sequence<T>* subSequence = new Sequence<T>();
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

    int GetLength() override{
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
            for (int i = 0; i < replacement->GetLength(); ++i) {
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
            if (currentChunk->GetLength() > 0) {
                newSequence->Concat(currentChunk);
            }
            return newSequence;
    }

    Sequence<T>* Append(T item) override{
        this->array->Resize(this->array->GetSize() + 1);
        this->array->Set(this->array->GetSize() - 1, item);
        return this;
    }

    Sequence<T>* Prepend(T item) override{
        this->array->Resize(this->array->GetSize() + 1);
        for (int i = this->array->GetSize() - 1; i > 0; --i) {
            this->array->Set(i, this->array->Get(i - 1));
        }
        this->array->Set(0, item);
        return this;
    }

    Sequence<T>* InsertAt(T item, int index) override{
        if (index < 0 || index >= this->array->GetSize()) {
            throw IndexOutOfRange();
        }
        this->array->Resize(this->array->GetSize() + 1);
        for (int i = this->array->GetSize() - 1; i > index; --i) {
            this->array->Set(i, this->array->Get(i - 1));
        }
        this->array->Set(index, item);
        return this;
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
class ImmutableArraySequence : public Sequence<T> {
protected:
    DynamicArray<T>* array;
    virtual ImmutableArraySequence<T>* CreateImmutableArraySequence() = 0;
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
        Sequence<T>* subSequence = new Sequence<T>();
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

    int GetLength() override{
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
            for (int i = 0; i < replacement->GetLength(); ++i) {
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
            if (currentChunk->GetLength() > 0) {
                newSequence->Concat(currentChunk);
            }
            return newSequence;
    }

    Sequence<T>* Append(T item) override{
        this->array->Resize(this->array->GetSize() + 1);
        this->array->Set(this->array->GetSize() - 1, item);
        return this;
    }

    Sequence<T>* Prepend(T item) override{
        this->array->Resize(this->array->GetSize() + 1);
        for (int i = this->array->GetSize() - 1; i > 0; --i) {
            this->array->Set(i, this->array->Get(i - 1));
        }
        this->array->Set(0, item);
        return this;
    }

    Sequence<T>* InsertAt(T item, int index) override{
        if (index < 0 || index >= this->array->GetSize()) {
            throw IndexOutOfRange();
        }
        this->array->Resize(this->array->GetSize() + 1);
        for (int i = this->array->GetSize() - 1; i > index; --i) {
            this->array->Set(i, this->array->Get(i - 1));
        }
        this->array->Set(index, item);
        return this;
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
class MutableListSequence : public Sequence<T> {
protected:
    LinkedList<T>* list;
    virtual MutableListSequence<T>* CreateMutableListSequence() = 0;
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
        Sequence<T>* subSequence = new Sequence<T>();
        for (int i = startIndex; i < endIndex; i++) {
            subSequence->Append(Get(i));
        }
        return subSequence;
    }

    T GetFirst() override{
        if (list->GetLength() == 0) {
            throw IndexOutOfRange();
        }
        return list->Get(0);
    }

    T GetLast() override{
        if (list->GetLength() == 0) {
            throw IndexOutOfRange();
        }
        return list->GetLast();
    }

    T Get(int index) override{
        return list->Get(index);
    }

    int GetLength() override{
        return list->GetLength();
    }

    T& operator[](int index) override {
        return (*list)[index];
    }

    const T& operator[](int index) const override {
        return (*list)[index];
    }

    bool TryGet(int index, T& value) override{
        if (index < 0 || index >= list->GetLength()) {
            throw IndexOutOfRange();
        }
        value = list->Get(index);
        return true;
    }

    bool TryFind(function<bool(T)> predicate, T& value) override{
        for (int i = 0; i < list->GetLength(); i++) {
            if (predicate((*list)[i])) {
                value = list->Get(i);
                return true;
            }
        }
        return false;
    }

    Sequence<T>* Map(function<bool(T)> func) override{
        MutableListSequence<T>* newSequence = CreateMutableListSequence();
        for (int i = 0; i < list->GetLength(); ++i) {
            newSequence->Append(func(list->Get(i)));
        }
        return newSequence;
    }

    T Reduce(function<T(T, T)> func, T startValue) override{
        T result = startValue;
        for (int i = 0; i < list->GetLength(); ++i) {
            result = func(result, list->Get(i));
        }
        return result;
    }

    Sequence<T>* Where(function<bool(T)> predicate) override{
        MutableListSequence<T>* newSequence = CreateMutableListSequence();
        for (int i = 0; i < list->GetLength(); ++i) {
            newSequence->Append(list->Get(i));
        }
        return newSequence;
    }

    Sequence<T>* Zip (Sequence<T>* other, function<T(T, T)> func) override {
        MutableListSequence<T>* newSequence = CreateMutableListSequence();
        int minLength = min(list->GetLength(), other->GetLength());
        for (int i = 0; i < minLength; ++i) {
            newSequence->Append(func(list->Get(i), other->Get(i)));
        }
        return newSequence;
    }

    Sequence<T>* Slice(int index, int count, Sequence<T>* replacement) override {
        MutableListSequence<T>* newSequence = CreateMutableListSequence();
        if (index < 0) {
            index = list->GetLength() + index;
            if (index < 0) {
                throw IndexOutOfRange();
            }
        }
        if (index >= list->GetLength() || index + count > list->GetLength()) {
            throw IndexOutOfRange();
        }
        for (int i = 0; i < index; ++i) {
            newSequence->Append(list->Get(i));
        }
        if (replacement != nullptr) {
            for (int i = 0; i < replacement->GetLength(); ++i) {
                newSequence->Append(replacement->Get(i));
            }
        }
        for (int i = index + count; i < list->GetLength(); ++i) {
            newSequence->Append(list->Get(i));
        }
        return newSequence;
    }

    Sequence<T>* Split(function<bool(T)> predicate) override {
        MutableListSequence<T>* newSequence = CreateMutableListSequence();
        MutableListSequence<T>* currentChunk = CreateMutableListSequence();
        for (int i = 0; i < list->GetLength(); ++i) {
            T item = list->Get(i);
            if (predicate(item)) {
                if (currentChunk->GetLength() > 0) {
                    newSequence->Concat(currentChunk);
                    currentChunk = CreateMutableListSequence();
                }
            } else {
                    currentChunk->Append(item);
                }
            }
            if (currentChunk->GetLength() > 0) {
                newSequence->Concat(currentChunk);
            }
            return newSequence;
    }

    Sequence<T>* Append(T item) override{
        this->list->Append(item);
        return this;
    }

    Sequence<T>* Prepend(T item) override{
        this->list->Prepend(item);
        return this;
    }

    Sequence<T>* InsertAt(T item, int index) override{
        this->list->InsertAt(item, index);
        return this;
    }

    Sequence<T>* Concat(Sequence<T>* list) override{
        this->list->Concat(list);
        return this;
    }

};

template <class T>
class ImmutableListSequence : public Sequence<T> {
protected:
    LinkedList<T>* list;
    virtual ImmutableListSequence<T>* CreateImmutableListSequence() = 0;
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
        Sequence<T>* subSequence = new Sequence<T>();
        for (int i = startIndex; i < endIndex; i++) {
            subSequence->Append(Get(i));
        }
        return subSequence;
    }

    T GetFirst() override{
        if (list->GetLength() == 0) {
            throw IndexOutOfRange();
        }
        return list->Get(0);
    }

    T GetLast() override{
        if (list->GetLength() == 0) {
            throw IndexOutOfRange();
        }
        return list->GetLast();
    }

    T Get(int index) override{
        return list->Get(index);
    }

    int GetLength() override{
        return list->GetLength();
    }

    T& operator[](int index) override {
        return (*list)[index];
    }

    const T& operator[](int index) const override {
        return (*list)[index];
    }

    bool TryGet(int index, T& value) override{
        if (index < 0 || index >= list->GetLength()) {
            throw IndexOutOfRange();
        }
        value = list->Get(index);
        return true;
    }

    bool TryFind(function<bool(T)> predicate, T& value) override{
        for (int i = 0; i < list->GetLength(); i++) {
            if (predicate((*list)[i])) {
                value = list->Get(i);
                return true;
            }
        }
        return false;
    }

    Sequence<T>* Map(function<bool(T)> func) override{
        MutableListSequence<T>* newSequence = CreateImmutableListSequence();
        for (int i = 0; i < list->GetLength(); ++i) {
            newSequence->Append(func(list->Get(i)));
        }
        return newSequence;
    }

    T Reduce(function<T(T, T)> func, T startValue) override{
        T result = startValue;
        for (int i = 0; i < list->GetLength(); ++i) {
            result = func(result, list->Get(i));
        }
        return result;
    }

    Sequence<T>* Where(function<bool(T)> predicate) override{
        MutableListSequence<T>* newSequence = CreateImmutableListSequence();
        for (int i = 0; i < list->GetLength(); ++i) {
            newSequence->Append(list->Get(i));
        }
        return newSequence;
    }

    Sequence<T>* Zip (Sequence<T>* other, function<T(T, T)> func) override {
        MutableListSequence<T>* newSequence = CreateImmutableListSequence();
        int minLength = min(list->GetLength(), other->GetLength());
        for (int i = 0; i < minLength; ++i) {
            newSequence->Append(func(list->Get(i), other->Get(i)));
        }
        return newSequence;
    }

    Sequence<T>* Slice(int index, int count, Sequence<T>* replacement) override {
        MutableListSequence<T>* newSequence = CreateImmutableListSequence();
        if (index < 0) {
            index = list->GetLength() + index;
            if (index < 0) {
                throw IndexOutOfRange();
            }
        }
        if (index >= list->GetLength() || index + count > list->GetLength()) {
            throw IndexOutOfRange();
        }
        for (int i = 0; i < index; ++i) {
            newSequence->Append(list->Get(i));
        }
        if (replacement != nullptr) {
            for (int i = 0; i < replacement->GetLength(); ++i) {
                newSequence->Append(replacement->Get(i));
            }
        }
        for (int i = index + count; i < list->GetLength(); ++i) {
            newSequence->Append(list->Get(i));
        }
        return newSequence;
    }

    Sequence<T>* Split(function<bool(T)> predicate) override {
        MutableListSequence<T>* newSequence = CreateImmutableListSequence();
        MutableListSequence<T>* currentChunk = CreateImmutableListSequence();
        for (int i = 0; i < list->GetLength(); ++i) {
            T item = list->Get(i);
            if (predicate(item)) {
                if (currentChunk->GetLength() > 0) {
                    newSequence->Concat(currentChunk);
                    currentChunk = CreateImmutableListSequence();
                }
            } else {
                    currentChunk->Append(item);
                }
            }
            if (currentChunk->GetLength() > 0) {
                newSequence->Concat(currentChunk);
            }
            return newSequence;
    }

    Sequence<T>* Append(T item) override{
        this->list->Append(item);
        return this;
    }

    Sequence<T>* Prepend(T item) override{
        this->list->Prepend(item);
        return this;
    }

    Sequence<T>* InsertAt(T item, int index) override{
        this->list->InsertAt(item, index);
        return this;
    }

    Sequence<T>* Concat(Sequence<T>* list) override{
        this->list->Concat(list);
        return this;
    }
};

template <class T>
class AdaptiveSequence : public Sequence<T> {

};

template <class T>
class SegmentedList {

};

template <class T>
class ICollection<T> {

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