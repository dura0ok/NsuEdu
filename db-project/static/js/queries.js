import {TabulatorFull as Tabulator} from 'tabulator-tables';
import {showToast} from "./toasts";

document.querySelectorAll(".query-execute").forEach((btn) => {
    btn.addEventListener("click", (e) => {
        e.preventDefault();
        const form = btn.closest('form')
        const formData = new FormData(form)
        const currentUrl = window.location.href
        const resultWrapper = document.querySelector(".results")
        resultWrapper.innerHTML = ""
        console.log(resultWrapper)


        fetch(currentUrl, {
            method: "POST",
            body: formData
        }).then((response) => {
            return response.json();
        }).then((jsonData) => {
            if (jsonData.error) {
                showToast(jsonData.message, 'error')
                return
            }


            jsonData.results.forEach((result) => {
                const columns = result.columns
                const data = result.data
                const resultDiv = document.createElement('div');
                resultDiv.classList.add('query-result');
                resultWrapper.appendChild(resultDiv)
                new Tabulator(resultDiv, {
                    columns: columns,
                    data: data,
                    layout: "fitColumns",
                    pagination:true,
                    paginationSize:5,
                });

            })

        }).catch((error) => {
            console.error(error);
        });

    });
})