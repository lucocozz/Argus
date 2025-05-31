import Link from '@docusaurus/Link';

export default function ModernHero() {
  return (
    <header className="hero">
      <div className="container">
        <div style={{ textAlign: 'center', maxWidth: '800px', margin: '0 auto' }}>
          <img 
            src="/img/argus-logo.webp" 
            alt="Argus" 
            style={{
              height: '256px',
              width: 'auto',
              marginBottom: '1rem',
              marginTop: '-2rem',
              opacity: 1,
            }} 
          />
          
          <h1 className="hero__title">
            Modern C CLI Parser
          </h1>

          <p className="hero__subtitle">
            Elegant command-line argument parsing with a lot of advanced features,
            designed for C developers who want simplicity without sacrificing power.
          </p>
          
          <div style={{ 
            display: 'flex', 
            gap: '1rem', 
            justifyContent: 'center', 
            flexWrap: 'wrap',
            marginBottom: '3rem'
          }}>
            <Link
              className="button button--primary button--lg"
              to="/getting-started/intro">
              Get Started
            </Link>
            <Link
              className="button button--secondary button--lg"
              to="/api-reference/overview">
              API Reference
            </Link>
          </div>
        </div>
        
        <div style={{
          display: 'grid', 
          gridTemplateColumns: 'repeat(auto-fit, minmax(300px, 1fr))', 
          gap: '2rem',
          marginTop: '3rem'
        }}>
          <div className="feature-card">
            <h3>🛡️ Type Safe</h3>
            <p>Strong typing with automatic validation. No more manual parsing or runtime errors.</p>
          </div>
          
          <div className="feature-card">
            <h3>🎯 Declarative</h3>
            <p>Define your CLI once with expressive macros. Argus handles the rest.</p>
          </div>
          
          <div className="feature-card">
            <h3>🚀 Powerful</h3>
            <p>Subcommands, collections, environment variables, and validation built-in.</p>
          </div>
        </div>
      </div>
    </header>
  );
}
