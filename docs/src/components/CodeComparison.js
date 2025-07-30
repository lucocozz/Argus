import CodeBlock from '@theme/CodeBlock';

const oldWayCode = `#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    int opt, verbose = 0, port = 8080;
    char *output = "output.txt";
    char *input = NULL;
    
    struct option long_options[] = {
        {"verbose", no_argument, 0, 'v'},
        {"output", required_argument, 0, 'o'},
        {"port", required_argument, 0, 'p'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };
    
    while ((opt = getopt_long(argc, argv, "vo:p:h", long_options, NULL)) != -1) {
        switch (opt) {
            case 'v': verbose = 1; break;
            case 'o': output = optarg; break;
            case 'p': 
                port = atoi(optarg);
                if (port < 1 || port > 65535) {
                    fprintf(stderr, "Error: Port must be 1-65535\\n");
                    return 1;
                }
                break;
            case 'h':
                printf("Usage: %s [-v] [-o FILE] [-p PORT] INPUT\\n", argv[0]);
                printf("  -v, --verbose    Enable verbose output\\n");
                printf("  -o, --output     Output file (default: output.txt)\\n");
                printf("  -p, --port       Port number (1-65535)\\n");
                printf("  -h, --help       Show this help\\n");
                return 0;
            case '?':
                fprintf(stderr, "Try '%s --help' for more information.\\n", argv[0]);
                return 1;
        }
    }
    
    if (optind >= argc) {
        fprintf(stderr, "Error: Missing required INPUT file\\n");
        return 1;
    }
    input = argv[optind];
    
    // Finally ready to use the parsed arguments...
}`;

const argusWayCode = `#include <argus.h>

ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    VERSION_OPTION(),
    OPTION_FLAG(
        'v', "verbose",
        HELP("Enable verbose output")
    ),
    OPTION_STRING(
        'o', "output",
        HELP("Output file"),
        DEFAULT("output.txt")
    ),
    OPTION_INT(
        'p', "port",
        HELP("Port number"),
        DEFAULT(8080),
        VALIDATOR(V_RANGE(1, 65535))
    ),
    POSITIONAL_STRING(
        "input",
        HELP("Input file")
    ),
)

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "my_tool", "1.0.0");
    
    if (argus_parse(&argus, argc, argv) != ARGUS_SUCCESS)
        return 1;
    
    // Type-safe access - validation and help generation automatic
    int port = argus_get(&argus, "port").as_int;
    bool verbose = argus_get(&argus, "verbose").as_bool;
    const char *output = argus_get(&argus, "output").as_string;
    const char *input = argus_get(&argus, "input").as_string;
    
    argus_free(&argus);
    return 0;
}`;

export default function CodeComparison() {
  return (
    <section style={{ padding: '4rem 0', backgroundColor: 'rgba(0, 0, 0, 0.2)' }}>
      <div className="container">
        <div style={{
          display: 'grid',
          gridTemplateColumns: 'repeat(auto-fit, minmax(500px, 1fr))',
          gap: '2rem',
          maxWidth: '2400px',
          margin: '0 auto'
        }}>
          <div>
            <h3 style={{ 
              textAlign: 'center', 
              marginBottom: '1.5rem', 
              color: 'var(--terminal-text-dim)',
              fontSize: '1.1rem',
              fontFamily: 'var(--ifm-font-family-monospace)',
              textTransform: 'uppercase',
              letterSpacing: '0.05em'
            }}>
              // Traditional approach - manual everything
            </h3>
            <div style={{ opacity: '0.75' }}>
              <CodeBlock language="c">
                {oldWayCode}
              </CodeBlock>
            </div>
          </div>
          
          <div>
            <h3 style={{ 
              textAlign: 'center', 
              marginBottom: '1.5rem', 
              color: 'var(--terminal-green-bright)',
              fontSize: '1.1rem',
              fontFamily: 'var(--ifm-font-family-monospace)',
              textTransform: 'uppercase',
              letterSpacing: '0.05em'
            }}>
              // Argus way - declare once, get everything
            </h3>
            <div style={{ 
              border: '2px solid var(--terminal-green)',
              borderRadius: '8px',
              boxShadow: '0 0 20px rgba(0, 255, 65, 0.2)'
            }}>
              <CodeBlock language="c">
                {argusWayCode}
              </CodeBlock>
            </div>
          </div>
        </div>
      </div>

      <style jsx>{`
        @media (max-width: 1200px) {
          section [style*="gridTemplateColumns"] {
            grid-template-columns: 1fr !important;
            gap: 1.5rem !important;
          }
        }
      `}</style>
    </section>
  );
}
