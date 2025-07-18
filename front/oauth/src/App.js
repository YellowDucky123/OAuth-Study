import './App.css';

const click_handle = () => {
  window.location.href = "https://www.google.com"
}

function App() {
  return (
    <div className="App">
      <button onClick={click_handle}>
        google Oauth
      </button>
    </div>
  );
}

export default App;
