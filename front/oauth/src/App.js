import './App.css';

const click_handle = () => {
  window.location.href = "http://localhost:8080/OAuth2"
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
