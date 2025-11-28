#ifndef LUCAS_ArgumentParser_HEADER
#define LUCAS_ArgumentParser_HEADER
#include <stdio.h>
#include <string>
#include <map>
#include <vector>
#include <algorithm>

namespace Lucas
{
    enum UNEXPECTED_ARGUMENT
    {
        UNKNOWN_OPTION,
        VALUE_OVERFLOW
    };
    class ArgumentParserConfig
    {
    public:
        bool ignoreExec = true;
        bool overflowCall = false;
        bool overflowToRaw = true;
        bool unknownOptionCall = false;
    };

    class OptionConfig
    {
        int max;
        std::string desc;
        std::string al;
    public:
        std::string description() const
        {
            return desc;
        }
        int maxValue() const
        {
            return max;
        }
        std::string alias() const
        {
            return al;
        }
        /**
         * @param description The description of the argument
         * @param maxValue -1 for infinite values. 0 for no values, next argument will be treated as raw argument
         */
        OptionConfig(std::string alias = "", int maxValue = 1, std::string description = "")
            : max(maxValue), desc(std::move(description)), al(std::move(alias)) {}
    };

    class ArgumentParser
    {
        std::map<std::string, OptionConfig> options;
        std::map<std::string, OptionConfig> optionsAlias;
        std::map<std::string, std::vector<std::string>> data;
        std::vector<std::string> raw;
        ArgumentParserConfig ArgumentParserConfig;
        void (*unexpected)(std::string, UNEXPECTED_ARGUMENT);

    public:
        /**
         * @details Get Config of an option defined
         */
        OptionConfig getOption(std::string option) {
            return options[option];
        }
        /**
         * Get values of an option from argument
         * @param key The option, if not found return empty vector
         */
        std::vector<std::string> get(std::string option)
        {
            if (data.count(option) < 1)
            {
                return std::vector<std::string>();
            }
            return data[option];
        }

        /**
         * Set config for the ArgumentParser
         * @param config Config Object
         */
        void config(Lucas::ArgumentParserConfig config)
        {
            this->ArgumentParserConfig = config;
        }

        /**
         * Called when unexpection occurred
         * @param function Function to be called
         */
        void onUnexpected(void (*function)(std::string, UNEXPECTED_ARGUMENT))
        {
            this->unexpected = function;
        }

        /**
         * Get list of arguments with no option
         */
        std::vector<std::string> getRaw()
        {
            return raw;
        }

        /**
         * Define an option with it's config
         * @param key Option name
         * @param config Config of the option
         */
        void define(std::string key, OptionConfig config)
        {
            options.emplace(key, config);
            if (config.alias() != "")
                optionsAlias.emplace(config.alias(), config);
        }

        /**
         * Parse function, always called after everything is setup
         * @param argc argc parameter of main()
         * @param argv argv parameter of main()
         */
        void parse(int argc, char **argv)
        {
            bool keyInitialized = false;
            std::string key;
            std::vector<std::string> value;
            for (int i = 0; i < argc; i++)
            {
                if (ArgumentParserConfig.ignoreExec && i == 0)
                    continue;
                std::string arg = std::string(argv[i]);

                if (arg.substr(0, 2) == "--")
                {
                    if (keyInitialized)
                    {
                        data.emplace(key, value);
                        value.clear();
                        keyInitialized = false;
                    }
                    std::string tmpKey = arg.substr(2);
                    if (options.count(tmpKey))
                    {
                        key = arg.substr(2);
                        keyInitialized = true;
                        continue;
                    }
                    if (ArgumentParserConfig.unknownOptionCall)
                        unexpected(tmpKey, UNKNOWN_OPTION);
                }
                else if (arg[0] == '-')
                {
                    if (keyInitialized)
                    {
                        data.emplace(key, value);
                        value.clear();
                        key = "";
                        keyInitialized = false;
                    }
                    std::string tmpKey = arg.substr(1);
                    if (optionsAlias.count(tmpKey))
                    {
                        auto it = std::find_if(
                            options.begin(),
                            options.end(),
                            [&tmpKey](const auto &pair)
                            {
                                return pair.second.alias() == tmpKey; // check member variable
                            });
                        key = it->first;
                        keyInitialized = true;
                        continue;
                    }
                    unexpected(tmpKey, UNKNOWN_OPTION);
                }
                else
                {
                    if (keyInitialized)
                    {
                        if (value.size() < options[key].maxValue() || options[key].maxValue() == -1)
                        {
                            value.push_back(arg);
                            continue;
                        }
                        if (ArgumentParserConfig.overflowCall)
                        {
                            unexpected(key, VALUE_OVERFLOW);
                        }
                        if (ArgumentParserConfig.overflowToRaw)
                            raw.push_back(arg);
                        else
                            value.push_back(arg);
                    }
                    else
                        raw.push_back(arg);
                }
            }
            if (keyInitialized)
            {
                data.emplace(key, value);
                value.clear();
                keyInitialized = false;
            }
        }
    };
}
#endif