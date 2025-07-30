export default function WhyChooseSection() {
  return (
    <section style={{ padding: '6rem 0' }}>
      <div className="container">
        <div style={{ textAlign: 'center', marginBottom: '4rem' }}>
          <h2 style={{
            fontSize: '2.5rem',
            fontFamily: 'var(--ifm-font-family-monospace)',
            color: 'var(--terminal-green-bright)',
            textTransform: 'uppercase',
            letterSpacing: '0.05em',
            marginBottom: '1rem'
          }}>
            &gt; The Right Tool for the Job
          </h2>
          <p style={{
            fontSize: '1.1rem',
            color: 'var(--terminal-text-dim)',
            maxWidth: '600px',
            margin: '0 auto',
            fontFamily: 'var(--ifm-font-family-monospace)'
          }}>
            Every library has its sweet spot. Here's where each one shines:
          </p>
        </div>

        <div style={{
          display: 'grid',
          gridTemplateColumns: 'repeat(auto-fit, minmax(300px, 1fr))',
          gap: '2rem',
          maxWidth: '1200px',
          margin: '0 auto'
        }}>
          <div style={{
            padding: '2rem',
            border: '1px solid var(--terminal-gray)',
            borderRadius: '8px',
            backgroundColor: 'rgba(0, 0, 0, 0.3)',
            transition: 'all 0.3s ease'
          }}>
            <h3 style={{
              color: 'var(--terminal-text)',
              fontFamily: 'var(--ifm-font-family-monospace)',
              fontSize: '1.2rem',
              marginBottom: '1rem'
            }}>
              ⚡ getopt - Maximum Control
            </h3>
            <p style={{ color: 'var(--terminal-text-dim)', marginBottom: '0.5rem' }}>
              <strong style={{ color: 'var(--terminal-green)' }}>Perfect when:</strong> Every byte of performance counts, embedded systems, strict memory constraints
            </p>
            <p style={{ color: 'var(--terminal-text-dim)' }}>
              <strong style={{ color: '#ff6b6b' }}>You'll need:</strong> Manual validation, custom help text, extensive error handling code
            </p>
          </div>

          <div style={{
            padding: '2rem',
            border: '1px solid var(--terminal-gray)',
            borderRadius: '8px',
            backgroundColor: 'rgba(0, 0, 0, 0.3)',
            transition: 'all 0.3s ease'
          }}>
            <h3 style={{
              color: 'var(--terminal-text)',
              fontFamily: 'var(--ifm-font-family-monospace)',
              fontSize: '1.2rem',
              marginBottom: '1rem'
            }}>
              🐧 argp - GNU Standard
            </h3>
            <p style={{ color: 'var(--terminal-text-dim)', marginBottom: '0.5rem' }}>
              <strong style={{ color: 'var(--terminal-green)' }}>Perfect when:</strong> Building core GNU/Linux utilities, maximum compatibility with existing tools
            </p>
            <p style={{ color: 'var(--terminal-text-dim)' }}>
              <strong style={{ color: '#ff6b6b' }}>Limitation:</strong> Tied to GNU ecosystem, complex callback patterns for advanced features
            </p>
          </div>

          <div style={{
            padding: '2rem',
            border: '1px solid var(--terminal-gray)',
            borderRadius: '8px',
            backgroundColor: 'rgba(0, 0, 0, 0.3)',
            transition: 'all 0.3s ease'
          }}>
            <h3 style={{
              color: 'var(--terminal-text)',
              fontFamily: 'var(--ifm-font-family-monospace)',
              fontSize: '1.2rem',
              marginBottom: '1rem'
            }}>
              📋 argtable3 - Type Safety First
            </h3>
            <p style={{ color: 'var(--terminal-text-dim)', marginBottom: '0.5rem' }}>
              <strong style={{ color: 'var(--terminal-green)' }}>Perfect when:</strong> Legacy C compiler constraints, need strong typing without modern features
            </p>
            <p style={{ color: 'var(--terminal-text-dim)' }}>
              <strong style={{ color: '#ff6b6b' }}>Missing:</strong> No subcommands, manual environment handling, verbose initialization
            </p>
          </div>

          <div style={{
            padding: '2rem',
            border: '2px solid var(--terminal-green)',
            borderRadius: '8px',
            backgroundColor: 'rgba(0, 255, 65, 0.05)',
            position: 'relative',
            transition: 'all 0.3s ease',
            boxShadow: '0 0 20px rgba(0, 255, 65, 0.2)'
          }}>
            <div style={{
              position: 'absolute',
              top: '-12px',
              right: '16px',
              background: 'var(--terminal-green)',
              color: 'var(--terminal-black)',
              padding: '4px 12px',
              borderRadius: '12px',
              fontSize: '0.7rem',
              fontWeight: '600',
              fontFamily: 'var(--ifm-font-family-monospace)',
              textTransform: 'uppercase'
            }}>
              Our Pick
            </div>
            <h3 style={{
              color: 'var(--terminal-green-bright)',
              fontFamily: 'var(--ifm-font-family-monospace)',
              fontSize: '1.2rem',
              marginBottom: '1rem'
            }}>
              🚀 Argus - Developer First
            </h3>
            <p style={{ color: 'var(--terminal-text)', marginBottom: '0.5rem' }}>
              <strong style={{ color: 'var(--terminal-green-bright)' }}>Perfect when:</strong> You want to focus on your application logic, not argument parsing infrastructure
            </p>
            <p style={{ color: 'var(--terminal-text)' }}>
              <strong style={{ color: '#feca57' }}>Modern requirement:</strong> C11+ compiler needed, but you get everything built-in
            </p>
          </div>
        </div>
      </div>

      <style jsx>{`
        div[style*="border: 1px solid var(--terminal-gray)"]:hover {
          border-color: var(--terminal-green) !important;
          transform: translateY(-2px);
          box-shadow: 0 4px 20px rgba(0, 255, 65, 0.15) !important;
        }
        
        @media (max-width: 768px) {
          section [style*="gridTemplateColumns"] {
            grid-template-columns: 1fr !important;
          }
        }
      `}</style>
    </section>
  );
}
