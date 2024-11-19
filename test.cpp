// test.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <locale>
#include <codecvt>
#include <regex>
#include <format>

struct person
{
	int number{};

	std::wstring surname{};
	std::wstring name{};

	explicit person(int number, std::wstring& surname, std::wstring& name) :
		number(number),
		surname(surname),
		name(name) {}

	person(const person& other) :
		number(other.number),
		name(other.name),
		surname(other.surname) {}

	person(person&& other) noexcept :
		number(other.number),
		name(std::move(other.name)),
		surname(std::move(other.surname)) {}

	person& operator=(const person& other)
	{
		number = other.number;
		name = other.name;
		surname = other.surname;
		return *this;
	}

	person& operator=(person&& other) noexcept
	{
		number = other.number;
		name = std::move(other.name);
		surname = std::move(other.surname);
		return *this;
	}
};

class file_stream
{
private:
	std::wfstream stream{};

public:
	explicit file_stream(const char* name)
	{
		std::locale russian_locale("ru_RU.UTF-8");
		stream.open(name);

		if (stream.is_open())
			stream.imbue(russian_locale);
	}

	~file_stream()
	{
		stream.close();
	}

	std::wfstream& get()
	{
		return stream;
	}

	bool operator()()
	{
		return stream.is_open();
	}

	bool operator!()
	{
		return !stream.is_open();
	}
};

int main()
{
	setlocale(LC_ALL, "ru");

	file_stream stream{ "info.txt" };

	if (!stream)
	{
		std::cout << "Can't open file!" << std::endl;
		return -1;
	}

	std::vector<std::wstring> information{};

	// regex info:
	// [A-Z]...+ means that it should have only english or russian letters
	// \\d+ - in info should be digits only
	std::wregex format(L"^([A-Za-zА-Яа-яЁё]+( [A-Za-zА-Яа-яЁё]+)): (\\d+)$");
	std::wstring info{};

	while (std::getline(stream.get(), info))
	{
		if (std::regex_match(info, format))
			information.emplace_back(info);
	}

	if (information.empty())
	{
		std::cout << "Can't parse information!" << std::endl;
		return -1;
	}

	std::vector<person> persons{};
	persons.reserve(information.size());

	for (auto& i : information)
	{
		size_t first_offset = i.find(L' ');
		size_t second_offset = i.find(L':');

		std::wstring surname = i.substr(0, first_offset);
		std::wstring name = i.substr(first_offset + 1, second_offset - first_offset - 1);
		std::wstring number = i.substr(second_offset + 2);

		int num = std::stoi(number);
		persons.emplace_back(person{ num, surname, name });
	}

	information.clear();

	while (true)
	{
		int index{ -1 };
		std::cout << "Select sort: " << std::endl;
		std::cout << "1. By name" << std::endl;
		std::cout << "2. By surname" << std::endl;
		std::cout << "3. By numbers" << std::endl;
		std::cout << "4. No sorting (stop program)" << std::endl;

		std::cin >> index;

		if (!(index >= 1 && index <= 3))
		{
			std::cout << "Wrong index!" << std::endl;
			return -1;
		}

		if (index == 4)
		{
			std::cout << "Goodbye!" << std::endl;
			break;
		}

		std::vector<std::wstring> total_information{};

		switch (index)
		{
		case 1:
		{
			std::sort(persons.begin(), persons.end(), [](const person& a, const person& b) {
				return a.name < b.name;
			});

			for (const auto& person : persons)
			{
				std::wstring out = std::format(L"{} {}: {}", person.name, person.surname, person.number);
				total_information.emplace_back(out);
			}
		}
		break;
		case 2:
		{
			std::sort(persons.begin(), persons.end(), [](const person& a, const person& b) {
				return a.surname < b.surname;
			});

			for (const auto& person : persons)
			{
				std::wstring out = std::format(L"{} {}: {}", person.surname, person.name, person.number);
				total_information.emplace_back(out);
			}
		}
		break;
		case 3:
		{
			std::sort(persons.begin(), persons.end(), [](const person& a, const person& b) {
				return a.number < b.number;
			});

			for (const auto& person : persons)
			{
				std::wstring out = std::format(L"{}: {} {}", person.number, person.surname, person.name);
				total_information.emplace_back(out);
			}
		}
		break;
		}

		std::cout << std::endl;

		std::cout << "Sorting done!" << std::endl;
		std::cout << "Output: " << std::endl;

		for (auto& i : total_information)
			std::wcout << i << std::endl;

		std::cout << std::endl;
	}

	return 0;
}