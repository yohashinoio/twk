/**
 * These codes are licensed under LICNSE_NAME License.
 * See the LICENSE for details.
 *
 * Copyright (c) 2022 Hiramoto Ittou.
 */

#include <lapis/compile/main.hpp>
#include <lapis/codegen/codegen.hpp>
#include <lapis/jit/jit.hpp>
#include <lapis/parse/parser.hpp>
#include <lapis/option/option.hpp>
#include <lapis/support/file.hpp>
#include <lapis/support/utils.hpp>
#include <lapis/support/exception.hpp>

namespace program_options = boost::program_options;

namespace lapis::compile
{

static bool isBackNewline(const char* str) noexcept
{
  for (;;) {
    if (*str == '\0')
      return *--str == '\n';
    ++str;
  }

  unreachable();
}

static std::ostream& printHelp(std::ostream&          ostm,
                               const std::string_view command,
                               const program_options::options_description& desc)
{
  fmt::print(ostm, "Usage: {} [options] file...\n", command);
  return ostm << desc;
}

// Emit object file without error even if target does not exist.
static void emitFile(codegen::CodeGenerator&               generator,
                     const program_options::variables_map& vmap)
{
  if (vmap.contains("emit")) {
    const auto target = stringToLower(vmap["emit"].as<std::string>());

    if (target == "llvm") {
      generator.emitLlvmIRFiles();
      return;
    }
    else if (target == "asm") {
      generator.emitAssemblyFiles();
      return;
    }
  }

  generator.emitObjectFiles();
}

CompileResult main(const int argc, const char* const* const argv)
try {
  const auto desc = createOptionsDesc();

  const auto vmap = getVariableMap(desc, argc, argv);

  if (argc == 1) {
    printHelp(std::cerr, *argv, desc);
    std::exit(EXIT_SUCCESS);
  }
  if (vmap.contains("version")) {
    std::cout << getVersion() << std::endl;
    std::exit(EXIT_SUCCESS);
  }
  else if (vmap.contains("help")) {
    printHelp(std::cout, *argv, desc);
    std::exit(EXIT_SUCCESS);
  }

  std::vector<parse::Parser::Result> parse_results;

  for (const auto& file_path : getInputFiles(*argv, vmap)) {
    auto input = loadFile(*argv, file_path);

    parse::Parser parser{std::move(input), std::move(file_path)};

    parse_results.emplace_back(parser.getResult());
  }

  codegen::CodeGenerator code_generator{*argv,
                                        std::move(parse_results),
                                        vmap["Opt"].as<unsigned int>(),
                                        getRelocationModel(*argv, vmap)};

  if (vmap.contains("JIT")) {
    return {true, code_generator.doJIT()};
  }
  else {
    emitFile(code_generator, vmap);
    return {true, std::nullopt};
  }

  unreachable();
}
catch (const program_options::error& err) {
  // Error about command line options.
  std::cerr << formatError(*argv, err.what())
            << (isBackNewline(err.what()) ? "" : "\n") << std::flush;

  return {false, std::nullopt};
}
catch (const ErrorBase& err) {
  std::cerr << err.what() << (isBackNewline(err.what()) ? "" : "\n")
            << std::flush;

  return {false, std::nullopt};
}

} // namespace lapis::compile
