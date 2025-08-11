import Link from '@docusaurus/Link';

export default function FinalCTA() {
  return (
    <section style={{ 
      padding: '6rem 0',
      background: 'linear-gradient(135deg, rgba(0, 255, 65, 0.02) 0%, transparent 50%, rgba(0, 255, 65, 0.02) 100%)'
    }}>
      <div className="container">
        <div style={{ 
          textAlign: 'center',
          maxWidth: '700px',
          margin: '0 auto'
        }}>
          <div style={{
            fontSize: '3rem',
            marginBottom: '1.5rem',
            fontFamily: 'var(--ifm-font-family-monospace)',
            color: 'var(--terminal-green)'
          }}>
            &gt;_
          </div>
          
          <h2 style={{
            fontSize: '2.2rem',
            fontFamily: 'var(--ifm-font-family-monospace)',
            color: 'var(--terminal-green-bright)',
            marginBottom: '1rem',
            lineHeight: '1.2'
          }}>
            Ready to write less boilerplate?
          </h2>
          
          <p style={{
            fontSize: '1.1rem',
            color: 'var(--terminal-text-dim)',
            marginBottom: '2.5rem',
            fontFamily: 'var(--ifm-font-family-monospace)',
            lineHeight: '1.6'
          }}>
            Join the developers who chose declarative over imperative.<br/>
            Your future self will thank you.
          </p>

          <div style={{
            display: 'flex',
            gap: '1.5rem',
            justifyContent: 'center',
            flexWrap: 'wrap',
            marginBottom: '3rem'
          }}>
            <Link
              className="button button--primary button--lg"
              to="/getting-started/quickstart"
              style={{
                minWidth: '200px',
                fontSize: '1rem',
                padding: '0.75rem 2rem'
              }}>
              Start Building →
            </Link>
            <Link
              className="button button--secondary button--lg"
              to="/examples/simple-cli"
              style={{
                minWidth: '200px',
                fontSize: '1rem',
                padding: '0.75rem 2rem'
              }}>
              See Examples
            </Link>
          </div>

          <div style={{
            display: 'grid',
            gridTemplateColumns: 'repeat(auto-fit, minmax(150px, 1fr))',
            gap: '2rem',
            marginTop: '3rem',
            padding: '2rem',
            border: '1px solid var(--terminal-gray)',
            borderRadius: '8px',
            backgroundColor: 'rgba(0, 0, 0, 0.2)'
          }}>
            <div style={{ textAlign: 'center' }}>
              <div style={{
                fontSize: '1.5rem',
                color: 'var(--terminal-green-bright)',
                fontFamily: 'var(--ifm-font-family-monospace)',
                fontWeight: '600',
                marginBottom: '0.5rem'
              }}>
                Easy setup
              </div>
              <div style={{
                fontSize: '0.9rem',
                color: 'var(--terminal-text-dim)',
                fontFamily: 'var(--ifm-font-family-monospace)'
              }}>
                vcpkg, meson-wrap, xmake-repo
              </div>
            </div>

            <div style={{ textAlign: 'center' }}>
              <div style={{
                fontSize: '1.5rem',
                color: 'var(--terminal-green-bright)',
                fontFamily: 'var(--ifm-font-family-monospace)',
                fontWeight: '600',
                marginBottom: '0.5rem'
              }}>
                0
              </div>
              <div style={{
                fontSize: '0.9rem',
                color: 'var(--terminal-text-dim)',
                fontFamily: 'var(--ifm-font-family-monospace)'
              }}>
                dependencies*
              </div>
            </div>

            <div style={{ textAlign: 'center' }}>
              <div style={{
                fontSize: '1.5rem',
                color: 'var(--terminal-green-bright)',
                fontFamily: 'var(--ifm-font-family-monospace)',
                fontWeight: '600',
                marginBottom: '0.5rem'
              }}>
                All platforms
              </div>
              <div style={{
                fontSize: '0.9rem',
                color: 'var(--terminal-text-dim)',
                fontFamily: 'var(--ifm-font-family-monospace)'
              }}>
                Linux, macOS, Windows
              </div>
            </div>
          </div>

          <p style={{
            fontSize: '0.8rem',
            color: 'var(--terminal-text-dim)',
            marginTop: '1rem',
            fontFamily: 'var(--ifm-font-family-monospace)',
            fontStyle: 'italic'
          }}>
            * PCRE2 optional for regex validation
          </p>
        </div>
      </div>

      <style jsx>{`
        @media (max-width: 768px) {
          h2 {
            font-size: 1.8rem !important;
          }
          
          div[style*="display: flex"] {
            flex-direction: column;
            align-items: center;
          }
          
          div[style*="gridTemplateColumns"] {
            grid-template-columns: 1fr !important;
            gap: 1rem !important;
          }
        }
      `}</style>
    </section>
  );
}
