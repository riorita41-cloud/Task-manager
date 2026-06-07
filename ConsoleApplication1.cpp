#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <locale>
#include <cstdlib>

struct Task {
	int id;
	bool is_done;
	std::string title;
};

void renumber_tasks(std::vector<Task>& tasks) {
    for (size_t i = 0; i < tasks.size(); ++i) {
        tasks[i].id = static_cast<int>(i) + 1;
    }
}

void load_tasks(const std::string& filename, std::vector <Task>& tasks) {
	std::ifstream file(filename);
	if (!file.is_open()) {
		return;
	}

	std::string line;
	while (std::getline(file, line)) {
		if (line.empty()) {
			continue;
		}

		std::stringstream ss(line);
        std::string id_str, status_str, title;

		if (std::getline(ss, id_str, '|') &&
			std::getline(ss, status_str, '|') &&
			std::getline(ss, title)) {

			Task new_task;
			new_task.id = std::stoi(id_str);  
			new_task.is_done = (status_str == "1"); 
			new_task.title = title;
			tasks.push_back(new_task);
		}
	}
	file.close();
}

void save_tasks(const std::string& filename, const std::vector<Task>& tasks) {
	std::ofstream file(filename);
	for (size_t i = 0; i < tasks.size(); ++i) {
		file << tasks[i].id << "|"
			<< (tasks[i].is_done ? "1" : "0") << "|"
			<< tasks[i].title << "\n";
	}
	file.close();
}

void add_task(std::vector<Task>& tasks, const std::string& title) {
	Task new_task;
    new_task.id = static_cast<int>(tasks.size()) + 1;
	new_task.is_done = false;
	new_task.title = title;
	tasks.push_back(new_task);
}

void print_tasks(const std::vector<Task>& tasks) {
	if (tasks.empty()) {
		std::cout << "Список задач пуст.\n";
		return;
	}

	std::cout << "\nСписок задач\n";
	for (size_t i = 0; i < tasks.size(); ++i) {
		std::string status = tasks[i].is_done ? "[Выполнено]" : "[Не выполнено]";
		std::cout << tasks[i].id << ". " << status << " - " << tasks[i].title << "\n";
	}
	std::cout << "--------------------\n";
}

bool update_task_status(std::vector<Task>& tasks, int target_id, bool new_status) {
	for (size_t i = 0; i < tasks.size(); ++i) {
		if (tasks[i].id == target_id) {
			tasks[i].is_done = new_status;
			return true; 
		}
	}
	return false;
}

bool delete_task(std::vector<Task>& tasks, int target_id) {
	for (size_t i = 0; i < tasks.size(); ++i) {
		if (tasks[i].id == target_id) {
			tasks.erase(tasks.begin() + i);
            renumber_tasks(tasks);
			return true;
		}
	}
	return false;
}

int clear_completed_tasks(std::vector<Task>& tasks) {
	int deleted_count = 0;
	size_t i = 0;
	while (i < tasks.size()) {
		if (tasks[i].is_done) {
			tasks.erase(tasks.begin() + i);
			deleted_count++;
		}
		else {
			++i;
		}
	}
    renumber_tasks(tasks);
	return deleted_count;
}

int delete_all_tasks(std::vector<Task>& tasks) {
    int count = tasks.size();
    tasks.clear(); 
    return count;
}

void print_help() {
	std::cout << "\nДоступные команды:\n";
	std::cout << "  add <название>  - добавить задачу\n";
	std::cout << "  list            - показать все задачи\n";
	std::cout << "  done <номер>    - отметить задачу выполненной\n";
	std::cout << "  undone <номер>  - вернуть задачу в статус 'не выполнена'\n";
	std::cout << "  delete <номер>  - удалить задачу\n";
    std::cout << "  delete_all      - удалить все задачи\n";
	std::cout << "  clear           - удалить все выполненные задачи\n";
	std::cout << "  help            - показать это сообщение\n";
	std::cout << "  exit            - выйти из программы\n\n";
}
int main() {
    setlocale(LC_ALL, "Russian");
    system("chcp 1251 > nul");

    std::vector<Task> tasks;
    const std::string filename = "tasks.txt";

    load_tasks(filename, tasks);

    std::cout << "Добро пожаловать в Менеджер Задач!\n";
    print_help();

    while (true) {
        std::cout << "Введите команду: ";
        std::string input;
        std::getline(std::cin, input);

        if (input.empty()) {
            continue;
        }

        std::stringstream ss(input);
        std::string command;
        ss >> command;

        if (command == "exit") {
            std::cout << "Сохранение данных и выход...\n";
            break;
        }
        else if (command == "help") {
            print_help();
        }
        else if (command == "list") {
            print_tasks(tasks);
        }
        else if (command == "add") {
            std::string title;
            std::getline(ss, title);

            if (!title.empty() && title[0] == ' ') {
                title = title.substr(1);
            }

            if (title.empty()) {
                std::cout << "Ошибка: укажите название задачи.\n";
            }
            else {
                add_task(tasks, title);
                save_tasks(filename, tasks);
                std::cout << "Задача успешно добавлена.\n";
            }
        }

        else if (command == "done" || command == "undone" || command == "delete") {
            std::string id_str;
            ss >> id_str;

            if (id_str.empty()) {
                std::cout << "Ошибка: укажите номер задачи.\n";
            } else {
                try {
                    int target_id = std::stoi(id_str);
                    bool success = false;

                    if (command == "done") {
                        success = update_task_status(tasks, target_id, true);
                    } else if (command == "undone") {
                        success = update_task_status(tasks, target_id, false);
                    } else if (command == "delete") {
                        success = delete_task(tasks, target_id);
                    }

                    if (success) {
                        save_tasks(filename, tasks);
                         std::cout << "Успешно.\n";
                    } else {
                        std::cout << "Ошибка: задача с номером " << target_id << " не найдена.\n";
                    }
                } catch (...) {
                    std::cout << "Ошибка: введите корректный номер.\n";
                }
            }
        }
        else if (command == "delete_all") {
             std::cout << "Вы уверены, что хотите удалить ВСЕ задачи? (y/n): ";
             std::string confirm;
             std::getline(std::cin, confirm);

             if (confirm == "y" || confirm == "Y") {
                 int count = delete_all_tasks(tasks);
                 save_tasks(filename, tasks);
                 std::cout << "Удалено задач: " << count << ". Список полностью очищен.\n";
             } else {
                 std::cout << "Операция отменена.\n";
             }
            
        }
        else if (command == "clear") {
             std::cout << "Вы уверены, что хотите удалить все выполненные задачи? (y/n): ";
             std::string confirm;
             std::getline(std::cin, confirm);

             if (confirm == "y" || confirm == "Y") {
                 int count = clear_completed_tasks(tasks);
                 save_tasks(filename, tasks);
                 std::cout << "Удалено задач: " << count << "\n";
             } else {
                 std::cout << "Операция отменена.\n";
             }
        }
        else {
            std::cout << "Неизвестная команда. Введите 'help' для справки.\n";
        }
    }

    save_tasks(filename, tasks);
    return 0;
}
